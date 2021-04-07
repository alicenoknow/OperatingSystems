# Signals

## Task 1. (30 %)
Write program that demonstrates inheritance of signals handling. Check whether mask, handlers, pending signals are being inherited after fork and exec.
Perform tests for SIGUSR1 and other signals. Program should take two arguments [fork/exec] and [ignore/mask/pending/handler]. Create raport.txt with your conclusions and results of tests.
## Task 2. (20%)
## Task 3. (50%)
Write two programs sender and catcher. Sender takes as arguments: pid of catcher, number of signals to send and mode [kill/sigrt/sigqueue]. Catcher takes only mode argument. 
- Sender sends given number of signals to catcher with function depending on mode. After sendnig all signals, should send sencond signal as an information that it was the last signal. Catcher after receivnig the secondd signal, should one send back the same number of signals, that was received. Both catcher and sender should print summary of number of signals, received and sent. In mode "sigqueue", sender should check if catcher has received the signal.
- Sender sends only one signal at the time and then waits for signal back from catcher. The rest stays the same.
## Results
### Task 1,3 - complete
Makefile - commands:
- make compile - compiles a program
- make run - runs program with arguments specified in a Makefile
- make clean - remove all the unneccessary files

