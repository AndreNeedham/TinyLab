namespace SerialXferSender
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.serialPort1 = new System.IO.Ports.SerialPort(this.components);
            this.sendFileButton = new System.Windows.Forms.Button();
            this.openFileDialog1 = new System.Windows.Forms.OpenFileDialog();
            this.listBox1 = new System.Windows.Forms.ListBox();
            this.getFileListButton = new System.Windows.Forms.Button();
            this.getSelectedFileButton = new System.Windows.Forms.Button();
            this.saveFileDialog1 = new System.Windows.Forms.SaveFileDialog();
            this.deleteFileButton = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // serialPort1
            // 
            this.serialPort1.BaudRate = 115200;
            this.serialPort1.DtrEnable = true;
            this.serialPort1.PortName = "COM3";
            // 
            // sendFileButton
            // 
            this.sendFileButton.Location = new System.Drawing.Point(270, 12);
            this.sendFileButton.Name = "sendFileButton";
            this.sendFileButton.Size = new System.Drawing.Size(140, 30);
            this.sendFileButton.TabIndex = 0;
            this.sendFileButton.Text = "Send a file";
            this.sendFileButton.UseVisualStyleBackColor = true;
            this.sendFileButton.Click += new System.EventHandler(this.sendFileButton_Click);
            // 
            // openFileDialog1
            // 
            this.openFileDialog1.FileName = "openFileDialog1";
            // 
            // listBox1
            // 
            this.listBox1.FormattingEnabled = true;
            this.listBox1.Location = new System.Drawing.Point(13, 12);
            this.listBox1.Name = "listBox1";
            this.listBox1.Size = new System.Drawing.Size(182, 212);
            this.listBox1.TabIndex = 1;
            // 
            // getFileListButton
            // 
            this.getFileListButton.Location = new System.Drawing.Point(270, 65);
            this.getFileListButton.Name = "getFileListButton";
            this.getFileListButton.Size = new System.Drawing.Size(140, 30);
            this.getFileListButton.TabIndex = 2;
            this.getFileListButton.Text = "Get file list";
            this.getFileListButton.UseVisualStyleBackColor = true;
            this.getFileListButton.Click += new System.EventHandler(this.getFileListButton_Click);
            // 
            // getSelectedFileButton
            // 
            this.getSelectedFileButton.Location = new System.Drawing.Point(270, 120);
            this.getSelectedFileButton.Name = "getSelectedFileButton";
            this.getSelectedFileButton.Size = new System.Drawing.Size(140, 28);
            this.getSelectedFileButton.TabIndex = 3;
            this.getSelectedFileButton.Text = "Get selected file";
            this.getSelectedFileButton.UseVisualStyleBackColor = true;
            this.getSelectedFileButton.Click += new System.EventHandler(this.getSelectedFileButton_Click);
            // 
            // deleteFileButton
            // 
            this.deleteFileButton.Location = new System.Drawing.Point(270, 172);
            this.deleteFileButton.Name = "deleteFileButton";
            this.deleteFileButton.Size = new System.Drawing.Size(140, 28);
            this.deleteFileButton.TabIndex = 4;
            this.deleteFileButton.Text = "Delete selected file";
            this.deleteFileButton.UseVisualStyleBackColor = true;
            this.deleteFileButton.Click += new System.EventHandler(this.deleteFileButton_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(422, 245);
            this.Controls.Add(this.deleteFileButton);
            this.Controls.Add(this.getSelectedFileButton);
            this.Controls.Add(this.getFileListButton);
            this.Controls.Add(this.listBox1);
            this.Controls.Add(this.sendFileButton);
            this.Name = "Form1";
            this.Text = "Tinylab SD transfer";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.ResumeLayout(false);

        }

        #endregion

        private System.IO.Ports.SerialPort serialPort1;
        private System.Windows.Forms.Button sendFileButton;
        private System.Windows.Forms.OpenFileDialog openFileDialog1;
        private System.Windows.Forms.ListBox listBox1;
        private System.Windows.Forms.Button getFileListButton;
        private System.Windows.Forms.Button getSelectedFileButton;
        private System.Windows.Forms.SaveFileDialog saveFileDialog1;
        private System.Windows.Forms.Button deleteFileButton;
    }
}

