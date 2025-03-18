# Readme
Simple project to see how MinHook library works.

The purpose of this is creating a simple tool that hooks: socket, connect, closesocket and send (all from winsocks) in order to inspect suspicious processes. As a result of the hooking process, a log file, named commInject.log, is created in %programmdata% with information related to network communications: socket creation, ip address and remote port connection, buffer sended to destination and socket closing.

There are 4 folders:

helloc2: Simple program used to test the hooking funcionality. helloc2 opens a tcp connection using sockets. helloc2 <ip_address>
autohook_helloc2: Poc, Here I'm using MinHook library as a first step to test if the implementation is correct.
hookcomms: DLL where the hooking process is implemented.
injector: Simple dll injector by creating a remotethread
The 'executables' folder is where I have my x86 and x64 release compiled version.

Example of the test:

cmd_1> helloc2.exe 192.168.1.30 443 cmd_2> injector.exe helloc2_PID %complete_dll_path%\hookcomms.dll
