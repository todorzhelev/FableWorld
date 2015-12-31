using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace DialogueEditor
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();

            inputText.KeyDown += new KeyEventHandler(NodeText_KeyDown);
            //inputText.KeyDown += new KeyEventArgs(NodeText_KeyDown);
        }

        private void treeView1_AfterSelect(object sender, TreeViewEventArgs e)
        {
            selectedTreeNode = e.Node;

            inputText.Text = selectedTreeNode.Text;
        }

        private void newToolStripMenuItem_Click(object sender, EventArgs e)
        {
            treeView1.Nodes.Clear();
            treeView1.Nodes.Add("Root");
            treeView1.SelectedNode = treeView1.Nodes[0];
        }

        private void button1_Click(object sender, EventArgs e)
        {
            treeView1.Nodes.Add("OMG BRAT");
            treeView1.Nodes[1].Nodes.Add("ebasi");
        }

        private void RemoveNode_Click(object sender, EventArgs e)
        {
            if(treeView1.SelectedNode != null )
            {
                treeView1.SelectedNode.Remove();
            }
        }

        private void AddChildNode_Click(object sender, EventArgs e)
        {
            if( treeView1.Nodes.Count == 0)
            {
                treeView1.Nodes.Add("Root");
                treeView1.SelectedNode = treeView1.Nodes[0];
            }
            else
            {
                treeView1.SelectedNode.Nodes.Add("test");
                treeView1.SelectedNode.Expand();
            }
        }

        private void ExpandAll_Click(object sender, EventArgs e)
        {
            if(treeView1.SelectedNode != null)
            {
                treeView1.SelectedNode.ExpandAll();
            }
        }

        private void NodeText_TextChanged(object sender, EventArgs e)
        {
        }

        private void NodeText_KeyDown(object sender, KeyEventArgs e)
        {
            var character = e.KeyData;

            if (e.KeyCode == Keys.Enter)
            {
                selectedTreeNode.Text = inputText.Text;
            }
            else
            {
                inputText.Text.Insert(inputText.Text.Length, e.KeyData.ToString());
            }
        }

        public TreeNode selectedTreeNode;

        private void retrieveInput_Click(object sender, EventArgs e)
        {
            selectedTreeNode.Text = inputText.Text;
        }
    }
}
