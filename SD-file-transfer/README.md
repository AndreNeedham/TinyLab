A quick app I wrote to transfer files from your PC to your Tinylab's SD card, and vice versa.

Sub-folders:

SerialXfer - Arduino sketch that handles loading/saving files onto the SD card.  Communicates through Serial/USB to the app below which runs on your PC.

SerialXferSender - WinForms app that communicates with the Tinylab via the USB cable.  Shows a list of files currently on the SD card and allows you to select files to copy to your PC or files to delete from the SD card.  You can also select files on the PC and copy them to the SD card.

More instructions for each app exist in the README.md of each sub-folder.
