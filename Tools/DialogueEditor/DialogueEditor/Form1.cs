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
            m_selectedTreeNode = e.Node;

            inputText.Text = m_selectedTreeNode.Text;
            questTextbox.Text = (string)m_selectedTreeNode.Tag;
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
                m_selectedTreeNode.Text = inputText.Text;
            }
            else
            {
                inputText.Text.Insert(inputText.Text.Length, e.KeyData.ToString());
            }
        }

        private void retrieveInput_Click(object sender, EventArgs e)
        {
            m_selectedTreeNode.Text = inputText.Text;
        }

        private void saveToolStripMenuItem_Click(object sender, EventArgs e)
        {
            ExportDialogue();
        }

        private void openToolStripMenuItem_Click(object sender, EventArgs e)
        {
            ImportDialogue();
        }

        private void modelTextbox_TextChanged(object sender, EventArgs e)
        {
        }

        private void modelTextbox_KeyDown(object sender, KeyEventArgs e)
        {
            var character = e.KeyData;

            if (e.KeyCode == Keys.Enter)
            {
                m_attachedToModel = modelTextbox.Text;
            }
            else
            {
                modelTextbox.Text.Insert(modelTextbox.Text.Length, e.KeyData.ToString());
            }
        }
  
        private void questTextbox_TextChanged(object sender, EventArgs e)
        {

        }

        private void questTextbox_KeyDown(object sender, KeyEventArgs e)
        {
            var character = e.KeyData;

            if (e.KeyCode == Keys.Enter)
            {
                m_selectedTreeNode.Tag = questTextbox.Text;
            }
            else
            {
                questTextbox.Text.Insert(questTextbox.Text.Length, e.KeyData.ToString());
            }
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////


       private string OpenFile()
        {
            DialogResult result = openFileDialog1.ShowDialog();

            string text = "";
            if (result == DialogResult.OK) // Test result.
            {
                text = openFileDialog1.FileName;
            }

            return text;
        }

        /// <summary>
        /// Imports dialogue from XML file
        /// </summary>
        private void ImportDialogue()
        {
            DialogResult result = openFileDialog1.ShowDialog();

            if (result == DialogResult.OK)
            {
                XmlDocument dialogueFile = new XmlDocument();
                XmlNode rootNode;

                FileStream fs = new FileStream(openFileDialog1.FileName, FileMode.Open, FileAccess.Read);
                dialogueFile.Load(fs);
                rootNode = dialogueFile.ChildNodes[1];

                var attachedToModelName = rootNode.Attributes.GetNamedItem("model").InnerText;
                modelTextbox.Text = attachedToModelName;

                treeView1.Nodes.Clear();

                treeView1.Nodes.Add(new TreeNode(dialogueFile.DocumentElement.Name));
                TreeNode tNode;
                tNode = treeView1.Nodes[0];
                AddTreeViewNodes(rootNode, tNode);

                fs.Close();
            }
        }

        /// <summary>
        /// creates tree view nodes based on the xml
        /// </summary>
        /// <param name="inXmlNode"></param>
        /// <param name="inTreeNode"></param>
        private void AddTreeViewNodes(XmlNode inXmlNode, TreeNode inTreeNode)
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
                    AddTreeViewNodes(xNode, tNode);
                }

                if (inXmlNode.Attributes.GetNamedItem("Text") != null)
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
                inTreeNode.Text = inXmlNode.Attributes.GetNamedItem("Text").InnerText;
                if (inXmlNode.Attributes.GetNamedItem("Quest") != null)
                {
                    inTreeNode.Tag = inXmlNode.Attributes.GetNamedItem("Quest").InnerText;
                }
            }
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////

        /// <summary>
        // Exports the current dialogue to xml file
        /// </summary>
        private void ExportDialogue()
        {
            var rootElementName = treeView1.Nodes[0].Text;
            var rootElement = new XElement(rootElementName, CreateXmlElements(treeView1.Nodes[0].Nodes));
            var attachedToModelName = new XAttribute("model", m_attachedToModel);
            rootElement.Add(attachedToModelName);

            DialogResult result = saveFileDialog1.ShowDialog();

            if (result == DialogResult.OK)
            {
                string filePath = saveFileDialog1.FileName;
                var document = new XDocument(rootElement);
                document.Save(filePath);
            }
        }

        /// <summary>
        /// creates xml elements, based on the nodes in the tree view
        /// </summary>
        /// <param name="treeViewNodes"></param>
        /// <returns></returns>
        private List<XElement> CreateXmlElements(TreeNodeCollection treeViewNodes)
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
                    element.Add(CreateXmlElements(treeViewNode.Nodes));
                }

                elements.Add(element);
            }
            return elements;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////


        private TreeNode m_selectedTreeNode;

        private string m_attachedToModel;
    }
}
