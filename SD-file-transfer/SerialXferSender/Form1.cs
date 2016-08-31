// SerialXferSender
// 2016 Andre Needham

// Winforms app that lets you transfer files to and from your Tinylab's SD card.  See README.md for more detailed instructions.

using System;
using System.IO;
using System.Management;
using System.Windows.Forms;

namespace SerialXferSender
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
            listBox1.Items.Clear();
        }

        String fileName;
        int fileSize;
        byte[] fileBytes;

        private void Form1_Load(object sender, EventArgs e)
        {
            string comPort = AutodetectArduinoPort();
            if (comPort != null)
            {
                serialPort1.PortName = comPort;
                getFileListButton_Click(sender, e);
            }
        }

        // Returns the Arduino's com port as "COM3" for example.   Thanks to StackOverflow user Brandon for the code example.
        private string AutodetectArduinoPort()
        {
            ManagementScope connectionScope = new ManagementScope();
            SelectQuery serialQuery = new SelectQuery("SELECT * FROM Win32_SerialPort");
            ManagementObjectSearcher searcher = new ManagementObjectSearcher(connectionScope, serialQuery);

            try
            {
                foreach (ManagementObject item in searcher.Get())
                {
                    string desc = item["Description"].ToString();
                    string deviceId = item["DeviceID"].ToString();

                    if (desc.Contains("Arduino"))
                    {
                        return deviceId;
                    }
                }
            }
            catch (ManagementException)
            {
                /* Do Nothing */
            }

            return null;
        }

        private void sendFileButton_Click(object sender, EventArgs e)
        {
            DialogResult result = openFileDialog1.ShowDialog();
            if (result == DialogResult.OK) 
            {
                fileName = openFileDialog1.SafeFileName;

                if(fileName.Length > 12)
                {
                    MessageBox.Show("You cannot send files with long file names.  File names must be of the '8.3' type - 8 character name followed by '.' followed by 3 character extension.");
                    return;
                }
                fileBytes = File.ReadAllBytes(openFileDialog1.FileName);
                fileSize = fileBytes.Length;
            }

            if (!serialPort1.IsOpen)
            {
                DisableButtons();

                try
                {
                    serialPort1.Open();
                    serialPort1.WriteLine(fileName);
                    serialPort1.WriteLine(fileSize.ToString());

                    //BUGBUG getting unexpected IOException on files > 1mb.  Let's try sending only 1mb at a time.
                    int fileLoc = 0;
                    while (fileSize > 1048576)
                    {
                        serialPort1.Write(fileBytes, fileLoc, 1048576);
                        fileSize -= 1048576;
                        fileLoc += 1048576;
                    }
                    serialPort1.Write(fileBytes, fileLoc, fileSize);
                    serialPort1.Close();
                }
                catch(Exception)
                {
                    MessageBox.Show("There was an error. Please make sure that the correct port was selected, and the device, plugged in.");
                }

                EnableButtons();

                listBox1.Items.Clear();
                getFileListButton_Click(sender, e);
            }
        }

        private void getFileListButton_Click(object sender, EventArgs e)
        {
            bool done = false;
            listBox1.Items.Clear();
            if (!serialPort1.IsOpen)
            {
                try
                {
                    serialPort1.Open();
                    serialPort1.WriteLine("DIR");

                    serialPort1.ReadTimeout = 5000;
                    string fileName;
                    while(!done)
                    {
                        fileName = serialPort1.ReadLine();
                        if (fileName.StartsWith("<end>")) done = true;
                        else listBox1.Items.Add(fileName);
                    }
                    serialPort1.Close();
                }
                catch
                {
                    MessageBox.Show("There was an error. Please make sure that the correct port was selected, and the device, plugged in.");
                }
            }
        }

        private void getSelectedFileButton_Click(object sender, EventArgs e)
        {
            if (null == listBox1.SelectedItem) return;

            int fileSize = 0;
            if (!serialPort1.IsOpen)
            {
                try
                {
                    serialPort1.Open();
                    serialPort1.WriteLine("GET");
                    serialPort1.WriteLine(listBox1.SelectedItem.ToString());

                    serialPort1.ReadTimeout = 5000;
                    string fileSizeString = serialPort1.ReadLine();
                    fileSize = int.Parse(fileSizeString);
                }
                catch
                {
                    MessageBox.Show("There was an error. Please make sure that the correct port was selected, and the device, plugged in.");
                }

                if (fileSize > 0)
                {
                    DisableButtons();

                    byte[] bytes = new byte[fileSize];
                    for (int index = 0; index < fileSize; index++)
                        bytes[index] = (byte)serialPort1.ReadByte();

                    saveFileDialog1.OverwritePrompt = true;
                    DialogResult result = saveFileDialog1.ShowDialog();
                    if (result == DialogResult.OK)
                    {
                        //NOTE!!! this WILL overwrite the destination file if it exists.  Thus the overwrite prompt a few lines up from here.
                        File.WriteAllBytes(saveFileDialog1.FileName, bytes);
                    }

                    EnableButtons();
                }
                else MessageBox.Show("Error reading file.");

                serialPort1.Close();
            }
        }

        private void deleteFileButton_Click(object sender, EventArgs e)
        {
            if (null == listBox1.SelectedItem) return;

            fileName = listBox1.SelectedItem.ToString();
            char[] trimChars = {'\r', '\n'};
            if (DialogResult.Yes == MessageBox.Show("Are you sure you want to delete " + fileName.TrimEnd(trimChars) + "?", "Confirm SD file delete", MessageBoxButtons.YesNo))
            {
                if (!serialPort1.IsOpen)
                {
                    try
                    {
                        serialPort1.Open();
                        serialPort1.WriteLine("DEL");
                        serialPort1.WriteLine(fileName);
                        serialPort1.Close();
                    }
                    catch
                    {
                        MessageBox.Show("There was an error. Please make sure that the correct port was selected, and the device, plugged in.");
                    }

                    listBox1.Items.Clear();
                    getFileListButton_Click(sender, e);
                }
            }
        }

        private void DisableButtons()
        {
            deleteFileButton.Enabled = false;
            getFileListButton.Enabled = false;
            sendFileButton.Enabled = false;
            getSelectedFileButton.Enabled = false;
        }

        private void EnableButtons()
        {
            deleteFileButton.Enabled = true;
            getFileListButton.Enabled = true;
            sendFileButton.Enabled = true;
            getSelectedFileButton.Enabled = true;
        }
    }
}
