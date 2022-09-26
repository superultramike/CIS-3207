SYNOPSIS
	MYps is a program is simple version of the ps command. It is executed from the commandline 
	and has a variety of functions to display for any said process

DESCRIPTION
		As stated above the user can provide an variety of arguments to the program to execute.
	All user input is sourced through a while loop that calls the getopt function to seek a user input character
	Once getopt finds the specific character in the cmdline using a switch case, it will raise a flag to be
	referred to late once the beginning argument scanning is done. MYps has two main roads that the user can go on.
	The user can either input one PID and have it's info displayed or no PID can be provided and all the info of all
	processes will be displayed. I will explain these two main divergent "roads" below
	
	FUNCTION void process_all_processes(void)
			This function is parent function to find_valid_pids() which processes all PIDs for a given user.
		Process_all_processes takes in a given PID and uses the dirent data structure by first opening the /proc 
		filesystem using opendir. If /proc can't be opened, it will return a perror, but if not then it will 
		perform a while loop that recursively asks if there is content in /proc to be read. Inside the while loop,
		we ask if there is a int present aka a PID, if there is then it must examined to see if it's valid for processing. 
		
		REFER TO FIND_VALID_PIDS FOR NEXT STEP
		
			If find_valid_pids returns 1 (a valid PID) then we convert the current PID to an int and store it
		in the global p array and increment to store the next possible valid PID
		
	FUNCTION int find_valid_pids(char *charPID)
			This function is the child function to process_all_processes() which adds valid processes to global p array.
		find_valid_pids() takes in a given PID from /proc and returns if the given PID is valid (1) or not (0).
		First we obtain the UID of the specific PID by opening /proc/%s/loginuid and store it char uid. Next we obtain the
		"TRUE" UID by opening the proc file /proc/self/loginuid. We do some additional error checking to make sure we can open
		said files. Next we perform a while loop that constantly scans the TRUE UID file while there is still content. But since
		the only content present is the TRUE UID, we scan and acquire it to store in a string. Then we close both file pointers 
		since we are done scanning for information. Finally we strcmp the TRUE UID and the UID we just acquired from the given PID.
		If the two strings match then the PID is valid and returns true (1) and (0) if they don't match.
		
	FUNCTION void printUtime(int pid)
			This function prints the Utime given a specific PID. First we open the /proc/%d/stat file using a specific PID.
		Next we use a while loop to constantly scan the file while there is still content present. Inside this while loop we define
		the char token as a space " " character since each element in /proc//stat is seperated by a space. So if uTime is the 14th element
		in the /proc//stat file, we need to count up till the 13th element, then we know for certain that the next element is uTime.
		So once our tokenCount hits 13, we print the next token aka the uTime. Finally we close the file pointer since we aren't reading anymore.
	
	FUNCTION void printStime(int pid)
			This function prints the Stime given a specific PID. First we open the /proc/%d/stat file using a specific PID.
		Next we use a while loop to constantly scan the file while there is still content present. Inside this while loop we define
		the char token as a space " " character since each element in /proc//stat is seperated by a space. So if uTime is the 15th element
		in the /proc//stat file, we need to count up till the 14th element, then we know for certain that the next element is sTime.
		So once our tokenCount hits 14, we print the next token aka the uTime. Finally we close the file pointer since we aren't reading anymore.
		
	FUNCTION void print_cmdline(int pid)
			This function prints the argument list of a given PID. First we generate the name of the cmdline for the process. Next we read the
		contents of the file. Then we define length as the size of the entire cmdline argument and indicate a null byte at the end of the argument line (length).
		Next we loop over all arguments (which are seperated by NULL bytes) and print them as we go and increment.
		
	FUNCTION printStat(int pid)
			This function prints the state of a given PID. First we open /proc/%d/stat using a specific PID. Next we use fscanf to extract
		the 4th element of /proc//stat since that's where state is. But since we are using fscanf and not a while loop token method, we to scan
		the 1th, 2nd, and 3rd element to reach the 4th element (state). So once we extract the 4th element we display it. Finally we close the
		file pointer since we aren't reading anymore.
		
	FUNCTION printSize(int pid)
			This function prints the v-memory size of a given PID. First we open /proc/%d/statm using a specific PID. Next we use fscanf to
		extract the 1st element, since luckily v-memory size is the first element of /proc/%d/statm! Then we display the information and close
		the file pointer since we have our information.

RESULTS
	IF -p IS PRESENT
			Raise the pflag and execute the following if statements. If -s is present then run printStat(), if -U isn't present then	
		printUtime(), if -S is present then run printStime(), if -v is present then run printSize(), if -c wasn't present then run print_cmdline().
	
	IF -p ISN'T PRESENT
			Don't raise the pflag and execute the following if statements. First we find all the valid PIDs and store them in the global p array.
		Next we run a for loop that performs a number of if statements on each PID in p array. If -s is present then run printStat(), if -U isn't present then	
		printUtime(), if -S is present then run printStime(), if -v is present then run printSize(), if -c wasn't present then run print_cmdline().
		

TEST PLAN
	To focus on working effectively I need to reduce the content of said project 1 down to
	it's key components (aka the project features from the PDF assignment).

	#1 step is to implement -p <pid> (MEDIUM DIFFICULTY)
		This will display process information (although I'm not what process info specifically).
			I want to extract the pid that the user enters (through scanf) and then get data from said /proc/ file	
		If this user doesn't provide pid then just display information for all processes of the current user
			- Use /proc scanning function to see every PID
			- then determine UID
			- then compare UID to the /proc/[pid]/loginuid
				- if they are the same, it's a valid PID
				- if they aren't the same, then don't do anything
			I need a loop through every process present on said user "ps a" as an example and then delimit the first '\0' and print
			
			BECAUSE OF THIS CONDITION ALL FUNCTIONS BELOW WILL BE PRESENT INSIDE OF A LOOP, WHERE THE AMOUNT OF CYCLES IS DETERMINED
			BY HOW MANY PROCESSES ARE EXTRACTED FROM THIS INITIAL FUNCTION.
			
				So if there is one process detected then the loop in each function step below will run 1 time (and n-times)	
		
	#2 step is to implement -s (EASY DIFFICULTY)
		Display the single character state information about the process, which is found in /proc/[pid]/stat (it's the 3rd element)
			I want to use my fscanf() function to scan the first three elements of said state info and then print out just the 3rd element
				I HAVE to scan the first and second element (even though I don't use it) because fscanf doesn't allow you to cycle through
				file elements like you normally would in delimiting a file. But I'm doing it this strange way because it's incredible easy to do
				I WILL REFER TO THIS METHOD AS THE FSCANF METHOD

	#3 step is to implement -U (EASY DIFFICULTY)
		Do not display the amount of user time consumed so far by this process. This info is found in the stat file by looking at "stime" aka the 14th element
			If this option is chosen by the user, then a flag will be created to state that U is true. But IF U is true then -S will not run

	#4 step is to implement -S (MEDIUM DIFFICULTY)
		Display the amount of system time consumed so far by this process. This info is found in the stat file by looking at "stime" ake the 14th element
			If -U flag is true, then -S will not run
			If -U flag is false, then -S will run by doing using...
				strtok to create the space or '\0' delimiter and scan the file until the 13th space/element then print from the 14th to the 15th element
			
	#5 step is to implement -v (EASY DIFFICULTY)
		Display the amount of virtual memory currently used (in pages) by this program. This info is found in the statm file, by looking at the first ("size") field
			USE THE FSCANF METHOD
			
	#6 step is to implement -c (EASY DIFFICULTY)
		Do not display the command-line that started this program. This info is found in the cmdline file in the process's directory.
			I'm assuming you print this out by default so I will make -c a true flag if it is used
			And then implement a simple read file function that will only print if -c flag is false

	#7 step is to implement printing out the cmdline for each process (EASY DIFFICULTY)
		If -c flag is true, then this function doesn't run
		If -c flag is false, then this function does run
		
	All of these implementations will be conjoined into of the getopt use "switch" case which is excellent for handling commandline procedures like this
		Each switch case within main will contain a function that produces the solution/result for each step above.
		Additionally flags will be used as an entry test before functions even run (for -S and -c)
