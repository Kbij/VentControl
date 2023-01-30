
namespace FirmwareTest
{
    partial class FirmwareTest
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
            this.edtIp = new System.Windows.Forms.TextBox();
            this.edtLog = new System.Windows.Forms.TextBox();
            this.btnGo = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // edtIp
            // 
            this.edtIp.Location = new System.Drawing.Point(27, 26);
            this.edtIp.Name = "edtIp";
            this.edtIp.Size = new System.Drawing.Size(228, 22);
            this.edtIp.TabIndex = 0;
            this.edtIp.Text = "192.168.10.125";
            // 
            // edtLog
            // 
            this.edtLog.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.edtLog.Location = new System.Drawing.Point(27, 75);
            this.edtLog.Multiline = true;
            this.edtLog.Name = "edtLog";
            this.edtLog.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.edtLog.Size = new System.Drawing.Size(761, 363);
            this.edtLog.TabIndex = 1;
            // 
            // btnGo
            // 
            this.btnGo.Location = new System.Drawing.Point(262, 26);
            this.btnGo.Name = "btnGo";
            this.btnGo.Size = new System.Drawing.Size(107, 23);
            this.btnGo.TabIndex = 2;
            this.btnGo.Text = "Start Test";
            this.btnGo.UseVisualStyleBackColor = true;
            this.btnGo.Click += new System.EventHandler(this.btnGo_Click);
            // 
            // FirmwareTest
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(800, 450);
            this.Controls.Add(this.btnGo);
            this.Controls.Add(this.edtLog);
            this.Controls.Add(this.edtIp);
            this.Name = "FirmwareTest";
            this.Text = "Firmware TCP Test";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox edtIp;
        private System.Windows.Forms.TextBox edtLog;
        private System.Windows.Forms.Button btnGo;
    }
}

