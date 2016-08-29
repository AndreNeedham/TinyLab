Serial transfer program

Andre Needham

Two programs that allow you to transfer files from your PC to your Tinylab's SD card and vice-versa.  Part one is an Arduino sketch.  Part two is a Windows Forms app built with Visual Studio 2015.

Upload the sketch in the SerialXfer folder onto your Tinylab (leave it running with the USB cable plugged in), then run the WinForms app SerialXferSender.exe on your PC.  You'll see a list of the files currently on the Tinylab's SD card.

Click the "Send a file button" and you'll see an Open File dialog.  Select the file you'd like to upload to your Tinylab's SD card and click Open.

Click "Get file list" to see the most recent list of files on the SD card (though it really should keep in sync as you load/save/delete files).

Select a file in the listbox, then click "Get selected file" to copy a file from the Tinylab's SD card to your computer.  You'll see a Save File dialog.  Enter a location/file name.  If the file already exists, you'll see a prompt to ask if you want to overwrite.

Select a file in the listbox, then click "Delete selected file" to delete that file from the SD card.  You'll be asked to confirm your decision.

Some notes:

Filenames must be of the 8.3 format - 8 characters maximum followed by "." and a three-character file extension.  Long filenames aren't supported.

You can only see the root folder of the SD card.  You can't load/save to a subfolder.

You can't copy over files larger than 2gb (but why would you want to in any case?).

File transfer speed is a bit slow.  I tried a 1 MB file and it took almost a minute.

The PC-side program is a WinForms program (targeting Windows, tested on Windows 10 64-bit and Windows 7 32-bit).  You might be able to get it to run on Linux as a Mono app.  If you want to transfer files between a Mac and Tinylab, you'll have to solve that on your own.
