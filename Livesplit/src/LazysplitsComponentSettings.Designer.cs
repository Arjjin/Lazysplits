namespace LiveSplit.Lazysplits
{
    partial class LazysplitsComponentSettings
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

        #region Component Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.SharedDataDirButton = new System.Windows.Forms.Button();
            this.SharedDataDirText = new System.Windows.Forms.TextBox();
            this.SharedDataDirLabel = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // SharedDataDirButton
            // 
            this.SharedDataDirButton.Location = new System.Drawing.Point(380, 4);
            this.SharedDataDirButton.Name = "SharedDataDirButton";
            this.SharedDataDirButton.Size = new System.Drawing.Size(79, 21);
            this.SharedDataDirButton.TabIndex = 0;
            this.SharedDataDirButton.Text = "browse";
            this.SharedDataDirButton.UseVisualStyleBackColor = true;
            this.SharedDataDirButton.Click += new System.EventHandler(this.SharedDataDirButton_Click);
            // 
            // SharedDataDirText
            // 
            this.SharedDataDirText.Location = new System.Drawing.Point(109, 4);
            this.SharedDataDirText.Name = "SharedDataDirText";
            this.SharedDataDirText.ReadOnly = true;
            this.SharedDataDirText.Size = new System.Drawing.Size(265, 20);
            this.SharedDataDirText.TabIndex = 1;
            // 
            // SharedDataDirLabel
            // 
            this.SharedDataDirLabel.AutoSize = true;
            this.SharedDataDirLabel.Location = new System.Drawing.Point(3, 8);
            this.SharedDataDirLabel.Name = "SharedDataDirLabel";
            this.SharedDataDirLabel.Size = new System.Drawing.Size(100, 13);
            this.SharedDataDirLabel.TabIndex = 2;
            this.SharedDataDirLabel.Text = "Shared data folder :";
            // 
            // LazysplitsComponentSettings
            // 
            this.Controls.Add(this.SharedDataDirLabel);
            this.Controls.Add(this.SharedDataDirText);
            this.Controls.Add(this.SharedDataDirButton);
            this.Name = "LazysplitsComponentSettings";
            this.Size = new System.Drawing.Size(462, 229);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button SharedDataDirButton;
        private System.Windows.Forms.TextBox SharedDataDirText;
        private System.Windows.Forms.Label SharedDataDirLabel;
    }
}
