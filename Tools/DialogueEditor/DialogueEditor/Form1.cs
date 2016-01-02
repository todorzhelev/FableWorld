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

            inputText.KeyDown    += new KeyEventHandler(NodeText_KeyDown);
            modelTextbox.KeyDown += new KeyEventHandler(modelTextbox_KeyDown);
            questTextbox.KeyDown += new KeyEventHandler(questTextbox_KeyDown);

            Text = "Dialogue editor";
        }

        private void treeView1_AfterSelect(object sender, TreeViewEventArgs e)
        {
            selectedTreeNode = e.Node;

            inputText.Text = selectedTreeNode.Text;
            questTextbox.Text = (string)selectedTreeNode.Tag;
        }

        private void newToolStripMenuItem_Click(object sender, EventArgs e)
        {
        }

        private void button1_Click(object sender, EventArgs e)
        {
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
                treeView1.Nodes.Add(new TreeNode("Root"));
                treeView1.SelectedNode = treeView1.Nodes[0];
            }
            else
            {
                treeView1.SelectedNode.Nodes.Add(new TreeNode("node"));
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
            var attachedToModelName = new XAttribute("model", attachedToModel);
            rootElement.Add(attachedToModelName);

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

                if (treeViewNode.Tag != null)
                {
                    XAttribute quest = new XAttribute("Quest", (string)treeViewNode.Tag);
                    if ((string)treeViewNode.Tag != " ")
                    {
                        element.Add(quest);
                    }
                }

                if (treeViewNode.Nodes.Count > 0)
                {
                    element.Add(CreateXmlElement(treeViewNode.Nodes));
                }

                elements.Add(element);
            }
            return elements;
        }

        private void openToolStripMenuItem_Click(object sender, EventArgs e)
        {
            XmlDocument dialogueFile = new XmlDocument();
            XmlNode rootNode;
            FileStream fs = new FileStream("export.xml", FileMode.Open, FileAccess.Read);
            dialogueFile.Load(fs);
            rootNode = dialogueFile.ChildNodes[1];

            var attachedToModelName = rootNode.Attributes.GetNamedItem("model").InnerText;
            modelTextbox.Text = attachedToModelName;

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

                if (inXmlNode.Attributes.GetNamedItem("Quest") != null)
                {
                    inTreeNode.Tag = inXmlNode.Attributes.GetNamedItem("Quest").InnerText;
                }
            }
            else
            {
                inTreeNode.Text  = inXmlNode.Attributes.GetNamedItem("Text").InnerText;
                if (inXmlNode.Attributes.GetNamedItem("Quest") != null)
                {
                    inTreeNode.Tag = inXmlNode.Attributes.GetNamedItem("Quest").InnerText;
                }
            }
        }

        private void modelTextbox_TextChanged(object sender, EventArgs e)
        {
        }

        private void modelTextbox_KeyDown(object sender, KeyEventArgs e)
        {
            var character = e.KeyData;

            if (e.KeyCode == Keys.Enter)
            {
                attachedToModel = modelTextbox.Text;
            }
            else
            {
                modelTextbox.Text.Insert(modelTextbox.Text.Length, e.KeyData.ToString());
            }
        }

        public TreeNode selectedTreeNode;

        public string attachedToModel;

        private void questTextbox_TextChanged(object sender, EventArgs e)
        {

        }

        private void questTextbox_KeyDown(object sender, KeyEventArgs e)
        {
            var character = e.KeyData;

            if (e.KeyCode == Keys.Enter)
            {
                selectedTreeNode.Tag = questTextbox.Text;
            }
            else
            {
                questTextbox.Text.Insert(questTextbox.Text.Length, e.KeyData.ToString());
            }
        }
    }
}
