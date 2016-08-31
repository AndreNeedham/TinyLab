Serial transfer program

Andre Needham

Two programs that allow you to transfer files from your PC to your Tinylab's SD card and vice-versa.  Part one is an Arduino sketch.  Part two is a Windows Forms app built with Visual Studio 2015.

Upload this sketch onto your Tinylab (leave it running with the USB cable plugged in), then run SerialXferSender.exe on your PC.  You'll see a list of the files currently on the Tinylab's SD card.

Click the "Send a file button" and you'll see an Open File dialog.  Select the file you'd like to upload to your Tinylab's SD card and click Open.

Click "Get file list" to see the most recent list of files on the SD card (though it really should keep in sync as you load/save/delete files).

Select a file in the listbox, then click "Get selected file" to copy a file from the Tinylab's SD card to your computer.  You'll see a Save File dialog.  Enter a location/file name.  If the file already exists, you'll see a prompt to ask if you want to overwrite.

Select a file in the listbox, then click "Delete selected file" to delete that file from the SD card.  You'll be asked to confirm your decision.

Some notes:

Filenames must be of the 8.3 format - 8 characters maximum followed by "." and a three-character file extension.  Long filenames aren't supported.

You can only see the root folder of the SD card.  You can't load/save to a subfolder.

You can't copy over files larger than 2GB (but why would you want to in any case?).  Largest file I've tested so far is 32MB.

File transfer speed is a bit slow.  Copying a 5MB file to the SD card takes about 1.5 minutes.  Copying the same file from the SD card to the PC takes 1 minute.

The PC-side program is a WinForms program (targeting Windows, tested on Windows 10).  You might be able to get it to run on Linux as a Mono app.  If you want to transfer files between a Mac and Tinylab, you'll have to solve that on your own.
