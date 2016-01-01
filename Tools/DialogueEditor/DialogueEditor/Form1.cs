using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Xml.Linq;
using System.Xml;
using System.IO;

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

        private void retrieveInput_Click(object sender, EventArgs e)
        {
            selectedTreeNode.Text = inputText.Text;
        }

        private void saveToolStripMenuItem_Click(object sender, EventArgs e)
        {
            var rootElementName = treeView1.Nodes[0].Text;
            var rootElement = new XElement(rootElementName, CreateXmlElement(treeView1.Nodes[0].Nodes));
            var document = new XDocument(rootElement);
            document.Save("export.xml");
        }

        private static List<XElement> CreateXmlElement(TreeNodeCollection treeViewNodes)
        {
            var elements = new List<XElement>();
            foreach (TreeNode treeViewNode in treeViewNodes)
            {
                var element = new XElement("Node");
                XAttribute name = new XAttribute("Text", treeViewNode.Text);
                element.Add(name);

                if (treeViewNode.Nodes.Count > 0)
                {
                    element.Add(CreateXmlElement(treeViewNode.Nodes));
                }

                elements.Add(element);
            }
            return elements;
        }

        public TreeNode selectedTreeNode;

        private void openToolStripMenuItem_Click(object sender, EventArgs e)
        {
            XmlDocument dialogueFile = new XmlDocument();
            XmlNode rootNode;
            FileStream fs = new FileStream("export.xml", FileMode.Open, FileAccess.Read);
            dialogueFile.Load(fs);
            rootNode = dialogueFile.ChildNodes[1];
            treeView1.Nodes.Clear();
            treeView1.Nodes.Add(new TreeNode(dialogueFile.DocumentElement.Name));
            TreeNode tNode;
            tNode = treeView1.Nodes[0];
            AddTreeViewNode(rootNode, tNode);

            fs.Close();
        }

        private void AddTreeViewNode(XmlNode inXmlNode, TreeNode inTreeNode)
        {
            XmlNode xNode;
            TreeNode tNode;
            XmlNodeList nodeList;
            int i = 0;
            if (inXmlNode.HasChildNodes)
            {
                nodeList = inXmlNode.ChildNodes;
                for (i = 0; i <= nodeList.Count - 1; i++)
                {
                    xNode = inXmlNode.ChildNodes[i];
                    inTreeNode.Nodes.Add(new TreeNode(xNode.Name));
                    tNode = inTreeNode.Nodes[i];
                    AddTreeViewNode(xNode, tNode);
                }

                if(inXmlNode.Attributes.GetNamedItem("Text") != null )
                {
                    inTreeNode.Text = inXmlNode.Attributes.GetNamedItem("Text").InnerText;
                }
            }
            else
            {
                inTreeNode.Text = inXmlNode.Attributes.GetNamedItem("Text").InnerText;
            }
        }
    }
}
