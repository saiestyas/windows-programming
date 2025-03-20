#Readme
- Creates a new process in suspended mode (process we want to investigate)
- injects dll
- resume process

Calling example: 
  injector.exe <suspicious_process.exe> <inject.dll>
  injector.exe  C:\Windows\System32\calc.exe C:\Users\saiestyas\source\repos\dlls\hooks\utilities\messageBox\messageBox.dll

Use always complete paths bc there is no way to know for sure where is going to happen the execution.
