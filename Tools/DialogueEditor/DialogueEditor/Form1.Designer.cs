namespace DialogueEditor
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
            System.Windows.Forms.TreeNode treeNode11 = new System.Windows.Forms.TreeNode("You have to...");
            System.Windows.Forms.TreeNode treeNode12 = new System.Windows.Forms.TreeNode("You dont have to");
            System.Windows.Forms.TreeNode treeNode13 = new System.Windows.Forms.TreeNode("Hello", new System.Windows.Forms.TreeNode[] {
            treeNode11,
            treeNode12});
            System.Windows.Forms.TreeNode treeNode14 = new System.Windows.Forms.TreeNode("goodbye");
            System.Windows.Forms.TreeNode treeNode15 = new System.Windows.Forms.TreeNode("RootNode", new System.Windows.Forms.TreeNode[] {
            treeNode13,
            treeNode14});
            this.treeView1 = new System.Windows.Forms.TreeView();
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.newToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.openToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.saveToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.quitToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.helpToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.helpToolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.aboutToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.AddNode = new System.Windows.Forms.Button();
            this.RemoveNode = new System.Windows.Forms.Button();
            this.ExpandAll = new System.Windows.Forms.Button();
            this.inputText = new System.Windows.Forms.TextBox();
            this.modelTextbox = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.questTextbox = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.openFileDialog1 = new System.Windows.Forms.OpenFileDialog();
            this.saveFileDialog1 = new System.Windows.Forms.SaveFileDialog();
            this.menuStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // treeView1
            // 
            this.treeView1.AllowDrop = true;
            this.treeView1.Font = new System.Drawing.Font("Times New Roman", 14F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.treeView1.LabelEdit = true;
            this.treeView1.Location = new System.Drawing.Point(210, 39);
            this.treeView1.Name = "treeView1";
            treeNode11.Name = "Node3";
            treeNode11.Text = "You have to...";
            treeNode12.Name = "Node4";
            treeNode12.Text = "You dont have to";
            treeNode13.Name = "Hello";
            treeNode13.Text = "Hello";
            treeNode14.Name = "Node2";
            treeNode14.Text = "goodbye";
            treeNode15.Name = "RootNode";
            treeNode15.Tag = "Root";
            treeNode15.Text = "RootNode";
            this.treeView1.Nodes.AddRange(new System.Windows.Forms.TreeNode[] {
            treeNode15});
            this.treeView1.Size = new System.Drawing.Size(795, 432);
            this.treeView1.TabIndex = 0;
            this.treeView1.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.treeView1_AfterSelect);
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem,
            this.helpToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(1122, 24);
            this.menuStrip1.TabIndex = 1;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.newToolStripMenuItem,
            this.openToolStripMenuItem,
            this.saveToolStripMenuItem,
            this.quitToolStripMenuItem});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(37, 20);
            this.fileToolStripMenuItem.Text = "File";
            // 
            // newToolStripMenuItem
            // 
            this.newToolStripMenuItem.Name = "newToolStripMenuItem";
            this.newToolStripMenuItem.Size = new System.Drawing.Size(160, 22);
            this.newToolStripMenuItem.Text = "New Dialogue";
            this.newToolStripMenuItem.Click += new System.EventHandler(this.newToolStripMenuItem_Click);
            // 
            // openToolStripMenuItem
            // 
            this.openToolStripMenuItem.Name = "openToolStripMenuItem";
            this.openToolStripMenuItem.Size = new System.Drawing.Size(160, 22);
            this.openToolStripMenuItem.Text = "Import Dialogue";
            this.openToolStripMenuItem.Click += new System.EventHandler(this.openToolStripMenuItem_Click);
            // 
            // saveToolStripMenuItem
            // 
            this.saveToolStripMenuItem.Name = "saveToolStripMenuItem";
            this.saveToolStripMenuItem.Size = new System.Drawing.Size(160, 22);
            this.saveToolStripMenuItem.Text = "Eport Dialogue";
            this.saveToolStripMenuItem.Click += new System.EventHandler(this.saveToolStripMenuItem_Click);
            // 
            // quitToolStripMenuItem
            // 
            this.quitToolStripMenuItem.Name = "quitToolStripMenuItem";
            this.quitToolStripMenuItem.Size = new System.Drawing.Size(160, 22);
            this.quitToolStripMenuItem.Text = "Quit";
            // 
            // helpToolStripMenuItem
            // 
            this.helpToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.helpToolStripMenuItem1,
            this.aboutToolStripMenuItem});
            this.helpToolStripMenuItem.Name = "helpToolStripMenuItem";
            this.helpToolStripMenuItem.Size = new System.Drawing.Size(44, 20);
            this.helpToolStripMenuItem.Text = "Help";
            // 
            // helpToolStripMenuItem1
            // 
            this.helpToolStripMenuItem1.Name = "helpToolStripMenuItem1";
            this.helpToolStripMenuItem1.Size = new System.Drawing.Size(107, 22);
            this.helpToolStripMenuItem1.Text = "Help";
            // 
            // aboutToolStripMenuItem
            // 
            this.aboutToolStripMenuItem.Name = "aboutToolStripMenuItem";
            this.aboutToolStripMenuItem.Size = new System.Drawing.Size(107, 22);
            this.aboutToolStripMenuItem.Text = "About";
            // 
            // AddNode
            // 
            this.AddNode.Font = new System.Drawing.Font("Times New Roman", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.AddNode.Location = new System.Drawing.Point(12, 39);
            this.AddNode.Name = "AddNode";
            this.AddNode.Size = new System.Drawing.Size(125, 42);
            this.AddNode.TabIndex = 2;
            this.AddNode.Text = "Add Node";
            this.AddNode.UseVisualStyleBackColor = true;
            this.AddNode.Click += new System.EventHandler(this.AddChildNode_Click);
            // 
            // RemoveNode
            // 
            this.RemoveNode.Font = new System.Drawing.Font("Times New Roman", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.RemoveNode.Location = new System.Drawing.Point(12, 87);
            this.RemoveNode.Name = "RemoveNode";
            this.RemoveNode.Size = new System.Drawing.Size(125, 40);
            this.RemoveNode.TabIndex = 4;
            this.RemoveNode.Text = "Remove Node";
            this.RemoveNode.UseVisualStyleBackColor = true;
            this.RemoveNode.Click += new System.EventHandler(this.RemoveNode_Click);
            // 
            // ExpandAll
            // 
            this.ExpandAll.Font = new System.Drawing.Font("Times New Roman", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.ExpandAll.Location = new System.Drawing.Point(12, 133);
            this.ExpandAll.Name = "ExpandAll";
            this.ExpandAll.Size = new System.Drawing.Size(125, 40);
            this.ExpandAll.TabIndex = 5;
            this.ExpandAll.Text = "Expand Node";
            this.ExpandAll.UseVisualStyleBackColor = true;
            this.ExpandAll.Click += new System.EventHandler(this.ExpandAll_Click);
            // 
            // inputText
            // 
            this.inputText.AcceptsReturn = true;
            this.inputText.AcceptsTab = true;
            this.inputText.Font = new System.Drawing.Font("Times New Roman", 14F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.inputText.Location = new System.Drawing.Point(210, 503);
            this.inputText.Name = "inputText";
            this.inputText.Size = new System.Drawing.Size(795, 29);
            this.inputText.TabIndex = 6;
            this.inputText.TextChanged += new System.EventHandler(this.NodeText_TextChanged);
            // 
            // modelTextbox
            // 
            this.modelTextbox.Font = new System.Drawing.Font("Times New Roman", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.modelTextbox.Location = new System.Drawing.Point(12, 218);
            this.modelTextbox.Name = "modelTextbox";
            this.modelTextbox.Size = new System.Drawing.Size(192, 26);
            this.modelTextbox.TabIndex = 7;
            this.modelTextbox.TextChanged += new System.EventHandler(this.modelTextbox_TextChanged);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(9, 202);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(93, 13);
            this.label1.TabIndex = 8;
            this.label1.Text = "Attached to model";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(9, 262);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(35, 13);
            this.label2.TabIndex = 9;
            this.label2.Text = "Quest";
            // 
            // questTextbox
            // 
            this.questTextbox.Font = new System.Drawing.Font("Times New Roman", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.questTextbox.Location = new System.Drawing.Point(12, 278);
            this.questTextbox.Name = "questTextbox";
            this.questTextbox.Size = new System.Drawing.Size(192, 26);
            this.questTextbox.TabIndex = 10;
            this.questTextbox.TextChanged += new System.EventHandler(this.questTextbox_TextChanged);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(210, 484);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(69, 13);
            this.label3.TabIndex = 11;
            this.label3.Text = "Dialogue text";
            // 
            // openFileDialog1
            // 
            this.openFileDialog1.FileName = "openFileDialog1";
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1122, 589);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.questTextbox);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.modelTextbox);
            this.Controls.Add(this.inputText);
            this.Controls.Add(this.ExpandAll);
            this.Controls.Add(this.RemoveNode);
            this.Controls.Add(this.AddNode);
            this.Controls.Add(this.treeView1);
            this.Controls.Add(this.menuStrip1);
            this.MainMenuStrip = this.menuStrip1;
            this.Name = "Form1";
            this.Text = "Form1";
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TreeView treeView1;
        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem newToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem openToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem saveToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem quitToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem helpToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem helpToolStripMenuItem1;
        private System.Windows.Forms.ToolStripMenuItem aboutToolStripMenuItem;
        private System.Windows.Forms.Button AddNode;
        private System.Windows.Forms.Button RemoveNode;
        private System.Windows.Forms.Button ExpandAll;
        private System.Windows.Forms.TextBox inputText;
        private System.Windows.Forms.TextBox modelTextbox;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox questTextbox;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.OpenFileDialog openFileDialog1;
        private System.Windows.Forms.SaveFileDialog saveFileDialog1;
    }
}

