namespace UI
{
    partial class verifyFeatureForm
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
            this.guideLabel = new System.Windows.Forms.Label();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.button1 = new System.Windows.Forms.Button();
            this.recognizeHumanLabel = new System.Windows.Forms.Label();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            this.SuspendLayout();
            // 
            // guideLabel
            // 
            this.guideLabel.AutoSize = true;
            this.guideLabel.BackColor = System.Drawing.SystemColors.InactiveCaption;
            this.guideLabel.Font = new System.Drawing.Font("Times New Roman", 16.2F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.guideLabel.Location = new System.Drawing.Point(178, 38);
            this.guideLabel.Name = "guideLabel";
            this.guideLabel.Size = new System.Drawing.Size(555, 33);
            this.guideLabel.TabIndex = 0;
            this.guideLabel.Text = "Stand in front of the camera and press the button";
            // 
            // pictureBox1
            // 
            this.pictureBox1.Location = new System.Drawing.Point(78, 97);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(800, 600);
            this.pictureBox1.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.pictureBox1.TabIndex = 1;
            this.pictureBox1.TabStop = false;
            // 
            // button1
            // 
            this.button1.BackColor = System.Drawing.SystemColors.ButtonShadow;
            this.button1.Location = new System.Drawing.Point(421, 703);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(125, 63);
            this.button1.TabIndex = 2;
            this.button1.Text = "EXIT";
            this.button1.UseVisualStyleBackColor = false;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // recognizeHumanLabel
            // 
            this.recognizeHumanLabel.AutoSize = true;
            this.recognizeHumanLabel.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.recognizeHumanLabel.Font = new System.Drawing.Font("Times New Roman", 18F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.recognizeHumanLabel.Location = new System.Drawing.Point(439, 645);
            this.recognizeHumanLabel.Name = "recognizeHumanLabel";
            this.recognizeHumanLabel.Size = new System.Drawing.Size(65, 34);
            this.recognizeHumanLabel.TabIndex = 3;
            this.recognizeHumanLabel.Text = "start";
            // 
            // verifyFeatureForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(985, 862);
            this.Controls.Add(this.recognizeHumanLabel);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.pictureBox1);
            this.Controls.Add(this.guideLabel);
            this.Name = "verifyFeatureForm";
            this.Text = "Verifying";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.verifyFeatureForm_FormClosing);
            this.Load += new System.EventHandler(this.verifyFeatureForm_Load);
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label guideLabel;
        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.Label recognizeHumanLabel;
    }
}