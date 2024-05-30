namespace MQTTForm
{
    partial class Form1
    {
        /// <summary>
        ///  Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        ///  Clean up any resources being used.
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
        ///  Required method for Designer support - do not modify
        ///  the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            console = new ListBox();
            btnRedLedOn = new Button();
            btnRedLedOff = new Button();
            btnYellowLedOff = new Button();
            btnYellowLedOn = new Button();
            SuspendLayout();
            // 
            // console
            // 
            console.FormattingEnabled = true;
            console.ItemHeight = 20;
            console.Location = new Point(566, 63);
            console.Margin = new Padding(3, 4, 3, 4);
            console.Name = "console";
            console.Size = new Size(289, 424);
            console.TabIndex = 0;
            // 
            // btnRedLedOn
            // 
            btnRedLedOn.Location = new Point(82, 48);
            btnRedLedOn.Margin = new Padding(3, 4, 3, 4);
            btnRedLedOn.Name = "btnRedLedOn";
            btnRedLedOn.Size = new Size(94, 47);
            btnRedLedOn.TabIndex = 1;
            btnRedLedOn.Text = "Red led on";
            btnRedLedOn.UseVisualStyleBackColor = true;
            btnRedLedOn.Click += btnLedOn_Click;
            // 
            // btnRedLedOff
            // 
            btnRedLedOff.Location = new Point(194, 48);
            btnRedLedOff.Margin = new Padding(3, 4, 3, 4);
            btnRedLedOff.Name = "btnRedLedOff";
            btnRedLedOff.Size = new Size(105, 47);
            btnRedLedOff.TabIndex = 2;
            btnRedLedOff.Text = "red led off";
            btnRedLedOff.UseVisualStyleBackColor = true;
            btnRedLedOff.Click += btnLedOff_Click;
            // 
            // btnYellowLedOff
            // 
            btnYellowLedOff.Location = new Point(194, 129);
            btnYellowLedOff.Margin = new Padding(3, 4, 3, 4);
            btnYellowLedOff.Name = "btnYellowLedOff";
            btnYellowLedOff.Size = new Size(105, 47);
            btnYellowLedOff.TabIndex = 4;
            btnYellowLedOff.Text = "Yellow led off";
            btnYellowLedOff.UseVisualStyleBackColor = true;
            btnYellowLedOff.Click += btnYellowLedOff_Click;
            // 
            // btnYellowLedOn
            // 
            btnYellowLedOn.Location = new Point(82, 129);
            btnYellowLedOn.Margin = new Padding(3, 4, 3, 4);
            btnYellowLedOn.Name = "btnYellowLedOn";
            btnYellowLedOn.Size = new Size(94, 47);
            btnYellowLedOn.TabIndex = 3;
            btnYellowLedOn.Text = "yellow led off";
            btnYellowLedOn.UseVisualStyleBackColor = true;
            btnYellowLedOn.Click += btnYellowLedOn_Click;
            // 
            // Form1
            // 
            AutoScaleDimensions = new SizeF(8F, 20F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(914, 600);
            Controls.Add(btnYellowLedOff);
            Controls.Add(btnYellowLedOn);
            Controls.Add(btnRedLedOff);
            Controls.Add(btnRedLedOn);
            Controls.Add(console);
            Margin = new Padding(3, 4, 3, 4);
            Name = "Form1";
            Text = "Form1";
            Load += Form1_Load;
            ResumeLayout(false);
        }

        #endregion

        private ListBox console;
        private Button btnRedLedOn;
        private Button btnRedLedOff;
        private Button btnYellowLedOff;
        private Button btnYellowLedOn;
    }
}