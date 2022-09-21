Project 1 README Step Notation
(Read in fullscreen for best reading perspective)
(THIS DOCUMENT IS SUBJECT TO CHANGE ON ALL INSTANCES FROM NOW 9/13 TILL 9/28)

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
