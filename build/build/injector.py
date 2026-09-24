import tkinter as tk
from tkinter import filedialog, messagebox, ttk
import ctypes
from ctypes import wintypes
import sys
import os

# Windows API constants
PROCESS_ALL_ACCESS = 0x1F0FFF
MEM_COMMIT = 0x1000
MEM_RESERVE = 0x2000
PAGE_READWRITE = 0x04

# Load Windows DLLs
kernel32 = ctypes.WinDLL('kernel32', use_last_error=True)
psapi = ctypes.WinDLL('psapi', use_last_error=True)

# Define function signatures
OpenProcess = kernel32.OpenProcess
OpenProcess.argtypes = [wintypes.DWORD, wintypes.BOOL, wintypes.DWORD]
OpenProcess.restype = wintypes.HANDLE

VirtualAllocEx = kernel32.VirtualAllocEx
VirtualAllocEx.argtypes = [wintypes.HANDLE, wintypes.LPVOID, ctypes.c_size_t, wintypes.DWORD, wintypes.DWORD]
VirtualAllocEx.restype = wintypes.LPVOID

WriteProcessMemory = kernel32.WriteProcessMemory
WriteProcessMemory.argtypes = [wintypes.HANDLE, wintypes.LPVOID, wintypes.LPCVOID, ctypes.c_size_t, ctypes.POINTER(ctypes.c_size_t)]
WriteProcessMemory.restype = wintypes.BOOL

CreateRemoteThread = kernel32.CreateRemoteThread
CreateRemoteThread.argtypes = [wintypes.HANDLE, wintypes.LPVOID, ctypes.c_size_t, wintypes.LPVOID, wintypes.LPVOID, wintypes.DWORD, wintypes.LPDWORD]
CreateRemoteThread.restype = wintypes.HANDLE

GetModuleHandleW = kernel32.GetModuleHandleW
GetModuleHandleW.argtypes = [wintypes.LPCWSTR]
GetModuleHandleW.restype = wintypes.HMODULE

GetProcAddress = kernel32.GetProcAddress
GetProcAddress.argtypes = [wintypes.HMODULE, wintypes.LPCSTR]
GetProcAddress.restype = wintypes.LPVOID

CloseHandle = kernel32.CloseHandle
CloseHandle.argtypes = [wintypes.HANDLE]
CloseHandle.restype = wintypes.BOOL


class ProcessInfo:
    def __init__(self, pid, name):
        self.pid = pid
        self.name = name
    
    def __str__(self):
        return f"{self.name} (PID: {self.pid})"


def get_processes():
    """Get list of running processes"""
    processes = []
    
    # Enumerate processes
    max_processes = 1024
    process_ids = (wintypes.DWORD * max_processes)()
    cb_needed = wintypes.DWORD()
    
    if not psapi.EnumProcesses(ctypes.byref(process_ids), ctypes.sizeof(process_ids), ctypes.byref(cb_needed)):
        return processes
    
    num_processes = cb_needed.value // ctypes.sizeof(wintypes.DWORD)
    
    for i in range(num_processes):
        pid = process_ids[i]
        if pid == 0:
            continue
        
        # Try to open process
        h_process = OpenProcess(0x0410, False, pid)  # PROCESS_QUERY_INFORMATION | PROCESS_VM_READ
        if h_process:
            # Get process name
            h_module = wintypes.HMODULE()
            cb_needed = wintypes.DWORD()
            if psapi.EnumProcessModules(h_process, ctypes.byref(h_module), ctypes.sizeof(h_module), ctypes.byref(cb_needed)):
                name_buffer = ctypes.create_unicode_buffer(260)
                if psapi.GetModuleBaseNameW(h_process, h_module, name_buffer, ctypes.sizeof(name_buffer)):
                    processes.append(ProcessInfo(pid, name_buffer.value))
            
            CloseHandle(h_process)
    
    return sorted(processes, key=lambda x: x.name.lower())


def inject_dll(pid, dll_path):
    """Inject DLL into target process using LoadLibraryW"""
    try:
        # Strip quotes from path if present
        dll_path = dll_path.strip().strip('"').strip("'")
        
        # Check if DLL exists
        if not os.path.exists(dll_path):
            return False, "DLL file not found"
        
        # Convert DLL path to absolute path
        dll_path = os.path.abspath(dll_path)
        
        # Open target process
        h_process = OpenProcess(PROCESS_ALL_ACCESS, False, pid)
        if not h_process:
            return False, f"Failed to open process (PID: {pid}). Try running as Administrator."
        
        try:
            # Get LoadLibraryW address
            h_kernel32 = GetModuleHandleW("kernel32.dll")
            if not h_kernel32:
                return False, "Failed to get kernel32.dll handle"
            
            load_library_addr = GetProcAddress(h_kernel32, b"LoadLibraryW")
            if not load_library_addr:
                return False, "Failed to get LoadLibraryW address"
            
            # Allocate memory in target process
            dll_path_bytes = dll_path.encode('utf-16le') + b'\x00\x00'
            dll_path_size = len(dll_path_bytes)
            
            remote_memory = VirtualAllocEx(h_process, None, dll_path_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE)
            if not remote_memory:
                return False, "Failed to allocate memory in target process"
            
            # Write DLL path to target process
            bytes_written = ctypes.c_size_t()
            if not WriteProcessMemory(h_process, remote_memory, dll_path_bytes, dll_path_size, ctypes.byref(bytes_written)):
                return False, "Failed to write DLL path to target process"
            
            # Create remote thread to load DLL
            thread_id = wintypes.DWORD()
            h_thread = CreateRemoteThread(h_process, None, 0, load_library_addr, remote_memory, 0, ctypes.byref(thread_id))
            
            if not h_thread:
                return False, "Failed to create remote thread"
            
            # Wait for thread to finish
            kernel32.WaitForSingleObject(h_thread, 0xFFFFFFFF)  # INFINITE
            
            CloseHandle(h_thread)
            
            return True, "DLL injected successfully!"
        
        finally:
            CloseHandle(h_process)
    
    except Exception as e:
        return False, f"Injection error: {str(e)}"


class InjectorGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("DLL Injector")
        self.root.geometry("600x450")
        self.root.resizable(False, False)
        
        # Style
        style = ttk.Style()
        style.theme_use('clam')
        
        self.dll_path = tk.StringVar()
        self.selected_process = None
        self.processes = []
        
        self.create_widgets()
        self.refresh_processes()
    
    def create_widgets(self):
        # Main frame
        main_frame = ttk.Frame(self.root, padding="10")
        main_frame.grid(row=0, column=0, sticky=(tk.W, tk.E, tk.N, tk.S))
        
        # Process selection section
        process_label = ttk.Label(main_frame, text="Select Target Process:", font=('Arial', 10, 'bold'))
        process_label.grid(row=0, column=0, sticky=tk.W, pady=(0, 5))
        
        # Search box
        search_frame = ttk.Frame(main_frame)
        search_frame.grid(row=1, column=0, sticky=(tk.W, tk.E), pady=(0, 5))
        
        ttk.Label(search_frame, text="Search:").pack(side=tk.LEFT, padx=(0, 5))
        self.search_var = tk.StringVar()
        self.search_var.trace('w', self.filter_processes)
        search_entry = ttk.Entry(search_frame, textvariable=self.search_var, width=30)
        search_entry.pack(side=tk.LEFT, fill=tk.X, expand=True)
        
        # Process listbox with scrollbar
        list_frame = ttk.Frame(main_frame)
        list_frame.grid(row=2, column=0, sticky=(tk.W, tk.E, tk.N, tk.S), pady=(0, 10))
        
        scrollbar = ttk.Scrollbar(list_frame)
        scrollbar.pack(side=tk.RIGHT, fill=tk.Y)
        
        self.process_listbox = tk.Listbox(list_frame, height=10, yscrollcommand=scrollbar.set, font=('Consolas', 9))
        self.process_listbox.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        scrollbar.config(command=self.process_listbox.yview)
        
        self.process_listbox.bind('<<ListboxSelect>>', self.on_process_select)
        
        # Refresh button
        refresh_btn = ttk.Button(main_frame, text="🔄 Refresh Processes", command=self.refresh_processes)
        refresh_btn.grid(row=3, column=0, sticky=(tk.W, tk.E), pady=(0, 15))
        
        # DLL selection section
        dll_label = ttk.Label(main_frame, text="DLL Path:", font=('Arial', 10, 'bold'))
        dll_label.grid(row=4, column=0, sticky=tk.W, pady=(0, 5))
        
        dll_frame = ttk.Frame(main_frame)
        dll_frame.grid(row=5, column=0, sticky=(tk.W, tk.E), pady=(0, 15))
        
        dll_entry = ttk.Entry(dll_frame, textvariable=self.dll_path, width=50)
        dll_entry.pack(side=tk.LEFT, fill=tk.X, expand=True, padx=(0, 5))
        
        browse_btn = ttk.Button(dll_frame, text="Browse", command=self.browse_dll)
        browse_btn.pack(side=tk.RIGHT)
        
        # Inject button
        inject_btn = ttk.Button(main_frame, text="💉 INJECT", command=self.inject, style='Accent.TButton')
        inject_btn.grid(row=6, column=0, sticky=(tk.W, tk.E), pady=(0, 10))
        
        # Configure accent button style
        style = ttk.Style()
        style.configure('Accent.TButton', font=('Arial', 12, 'bold'))
        
        # Status label
        self.status_label = ttk.Label(main_frame, text="Ready", foreground="green", font=('Arial', 9))
        self.status_label.grid(row=7, column=0, sticky=tk.W)
        
        # Configure grid weights
        main_frame.columnconfigure(0, weight=1)
        main_frame.rowconfigure(2, weight=1)
        self.root.columnconfigure(0, weight=1)
        self.root.rowconfigure(0, weight=1)
    
    def refresh_processes(self):
        """Refresh the process list"""
        self.status_label.config(text="Refreshing processes...", foreground="blue")
        self.root.update()
        
        self.processes = get_processes()
        self.filter_processes()
        
        self.status_label.config(text=f"Found {len(self.processes)} processes", foreground="green")
    
    def filter_processes(self, *args):
        """Filter processes based on search query"""
        search_query = self.search_var.get().lower()
        
        self.process_listbox.delete(0, tk.END)
        
        for proc in self.processes:
            if search_query in proc.name.lower():
                self.process_listbox.insert(tk.END, str(proc))
    
    def on_process_select(self, event):
        """Handle process selection"""
        selection = self.process_listbox.curselection()
        if selection:
            index = selection[0]
            display_text = self.process_listbox.get(index)
            
            # Find the actual process
            for proc in self.processes:
                if str(proc) == display_text:
                    self.selected_process = proc
                    self.status_label.config(text=f"Selected: {proc.name} (PID: {proc.pid})", foreground="blue")
                    break
    
    def browse_dll(self):
        """Open file dialog to select DLL"""
        filename = filedialog.askopenfilename(
            title="Select DLL file",
            filetypes=[("DLL files", "*.dll"), ("All files", "*.*")]
        )
        if filename:
            self.dll_path.set(filename)
            self.status_label.config(text=f"DLL selected: {os.path.basename(filename)}", foreground="green")
    
    def inject(self):
        """Perform DLL injection"""
        if not self.selected_process:
            messagebox.showerror("Error", "Please select a target process")
            return
        
        if not self.dll_path.get():
            messagebox.showerror("Error", "Please select a DLL file")
            return
        
        # Confirm injection
        confirm = messagebox.askyesno(
            "Confirm Injection",
            f"Inject DLL into {self.selected_process.name}?\n\nPID: {self.selected_process.pid}\nDLL: {os.path.basename(self.dll_path.get())}"
        )
        
        if not confirm:
            return
        
        self.status_label.config(text="Injecting...", foreground="orange")
        self.root.update()
        
        # Perform injection
        success, message = inject_dll(self.selected_process.pid, self.dll_path.get())
        
        if success:
            self.status_label.config(text=message, foreground="green")
            messagebox.showinfo("Success", message)
        else:
            self.status_label.config(text=f"Failed: {message}", foreground="red")
            messagebox.showerror("Injection Failed", message)


def main():
    # Check if running on Windows
    if sys.platform != 'win32':
        print("This injector only works on Windows")
        sys.exit(1)
    
    root = tk.Tk()
    app = InjectorGUI(root)
    root.mainloop()


if __name__ == "__main__":
    main()
