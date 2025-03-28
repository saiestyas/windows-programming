Prove of concept of a hooking dll that if injected in a process is able to log operation related to file creation, write or deletion and it also creates copies of the files created by the victim process.
Reminder: The injector, located in the utilities folder, is prepared to create a process in suspended mode, remotly inject the dll and resume the process. 

- Usage: 
injector.exe <fullpath_exe_under_test> <fullpath_dropper.hooks.dll>

Hooks:  CreateFileA/W, WriteFile, DeleteFileA/W and CloseHandle.

CreateFileA/W -> Creates a new log entry and also creates the same file in dropper_hooks folder (it's empty bc it's just file creation, nevertheless knowing that something was created it's useful in case any of the recovery file attemps files. As last resource, I encourage you to check if it's still in the creation  original path)
WriteFile     -> Creates a new log entry, the object (file) is occupied so any attempt of file manipulation ends with a violation access error, so, just logs. Any try of file creation (in this hook), take the handle and perform the write will result in a hook loop.
DeleteFileA/W -> Before deletion, copyfile and log operation.
CloseHandle   -> If there is a closehandle of a file, then the file is released and is accesible, so get a copy of it!


By default

 - Creates folder dropper_hooks in C:\ProgramData\  
 - In C:\ProgramData\dropper_hooks:
     -  dropper.logs (log file)
     - Copy of the files created by exe_under_test.
     - tmp files, empty or with some (or complete) content.
  
  If you want to adapt the paths, locations etc, edit globals.h (it's located in the headers folder)
