# Tutorial 

From the editor: This tutorial is to help users to preprocess the original routes.dat to an **updated** version that can be used in our graph program.

## Step 1: Mark the fileChange.py to executable state - Add `#! /usr/bin/python` at the top of the script and type `chomd a+x fileChange.py` in your terminal

- First, we need add "shebang line" at the top of our .py file to mark this file can be run by Python compiler.

  - Oh, wait! What's the "shebang line"?
  
  - The shebang line specifies where is the "path of python compiler". When we run it later, Linux will use the user-specified path to find the Python compiler and allow us can run it later.
  
  - The form of shebang line: `#! /usr/bin/python`
  
     - `#!` specifies the following path is the path that we are going to use
     
     - `/usr/bin/python` specifies the path
     
     - Please remember to leave a blank(` `) between `#!` and the path
  
  - But I don't know where is the path of the Python compiler.
  
  - Don't worry. Please type `whereis python` in your terminal. This command will help you to find the path of the Python compiler. You also can specify the version of Python. For instance, you can type `whereis python2.7` and this also can give you the answer.
  
- Second, we need to mark our current file to be executable.

   - To view whether our script is executable, try to run `ls -al FILENAME` in Linux. You also can run `ls -al` to view all of the states of your files in the current directory.
   
   - After typing this command, this will give you something like this `-rw-r--r--. 1 username ews 2376228 May 12 10:23 FILENAME`. We are only interested in `-rw-r--r--`.
   
   - If the file is a directory, you will see the first flag be `d`. For instance `drwxr-xr-x`.
   
   - How to view the file is executable? We need to avoid the first flag and evenly divide the rest of the flags into a group of size 3. Like this `rw-` or `rwx`
   
   - `r` represents the the right to read, `w` represents the the right to write, and `x` represents the the right to execute. If you see any of the flag is `-`, which means the corresponding user don't that right.
   
   - If you see `x`, you don't need to worry about anything. Otherwise, typing `chomd a+x fileChange.py` to allow everyone to have a right to execute this file. As this .py file is **trivial** and not so important, you can do this. In practice, I do not encourage you to do that because this may cause serious problems.
   
   - Outside resources: https://phoenixnap.com/kb/linux-file-permissions
   
## Step2: Type `./fileChange.py` in your terminal and you will get a file called `output.dat`

## Step3: Use `sort` Linux command.

   - Please use the flag `>` when you sort your file, otherwise it only print out the output to the terminal.
   
   - To be specific, you need to run `sort ORIGINAL.dat > NEWNAME.dat`

## Step4: Put the updated routes.dat to correct location to allow our program to produce a graph for you!

## Done. Thank you for your reading!!
