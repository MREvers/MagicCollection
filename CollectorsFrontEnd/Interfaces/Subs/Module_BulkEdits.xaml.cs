﻿using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using CollectorsFrontEnd.InterfaceModels;

namespace CollectorsFrontEnd.Interfaces.Subs
{
    /// <summary>
    /// Interaction logic for CompSubBulkEdits.xaml
    /// </summary>
    public partial class Module_BulkEdits : UserControl, IComponent, INotifyPropertyChanged
    {
        public ObservableCollection<string> LstTextChanges { get; set; }
        public string RemoveCardDisplay { get; set; }
        public string AddCardDisplay { get; set; }

        public event PropertyChangedEventHandler PropertyChanged;
        public event ComponentEvent UnhandledEvent;

        public class Data : IDataModel
        {
            public List<string> LstTextChanges = new List<string>();
        }

        public Data DataModel;
        private int m_iAddSelectedIndex = -1;
        private int m_iRemoveSelectedIndex = -1;
        private bool m_bIsOfferedSelectionAdd = false;
        private bool m_bIsOfferedSelectionRemove = false;
        private TextBox m_TBInAddCombo;
        private TextBox m_TBInRemoveCombo;
        private CollectionModel m_ColModel;

        #region Public Functions

        public Module_BulkEdits(CollectionModel aoColModel)
        {
            InitializeComponent();
            LstTextChanges = new ObservableCollection<string>();
            m_ColModel = aoColModel;
            DataContext = this;
            DataModel = new Data();

            CBAddCardSearch.AddHandler(System.Windows.Controls.Primitives.TextBoxBase.TextChangedEvent,
                      new System.Windows.Controls.TextChangedEventHandler(eAddCardSearch_TextChanged));
            CBAddCardSearch.SelectionChanged += eAddCardSearch_SelectionChanged;
            CBAddCardSearch.Focus();
            CBAddCardSearch.Loaded += eCBAddCardSearch_Loaded;
            CBAddCardSearch.LostFocus += eCBAddCardSearch_LostFocus;
            CBAddCardSearch.IsTextSearchEnabled = false;

            CBAddCardSearch.PreviewKeyDown += eTabOnAddCB_Press;

            CBRemoveCardSearch.AddHandler(System.Windows.Controls.Primitives.TextBoxBase.TextChangedEvent,
                      new System.Windows.Controls.TextChangedEventHandler(eRemoveCardSearch_TextChanged));

            CBRemoveCardSearch.SelectionChanged += eRemoveCardSearch_SelectionChanged;
            CBRemoveCardSearch.Loaded += eCBRemoveCardSearch_Loaded;
            CBRemoveCardSearch.LostFocus += eCBRemoveCardSearch_LostFocus;
            CBRemoveCardSearch.IsTextSearchEnabled = false;
        }

        public IDataModel GetDataModel()
        {
            throw new NotImplementedException();
        }

        public void RouteReceivedUnhandledEvent(IDataModel aDataObject, string aszAction)
        {
            throw new NotImplementedException();
        }

        #endregion

        #region UI Event Handler

        private void eAddItem_Click(object sender, RoutedEventArgs e)
        {
            if (m_bIsOfferedSelectionAdd)
            {
                string szCmdString = "";
                szCmdString += "+ " + m_TBInAddCombo.Text;
                LstTextChanges.Add(szCmdString);
                DataModel.LstTextChanges.Add(szCmdString);
                m_TBInAddCombo.Text = "";
                m_TBInRemoveCombo.Text = "";
                m_TBInAddCombo.Focus();
            }
        }

        private void eRemoveReplaceItem_Click(object sender, RoutedEventArgs e)
        {
            if (m_bIsOfferedSelectionRemove)
            {
                string szCmdString = "";
                string szDisplay = "";
                if (m_bIsOfferedSelectionAdd)
                {
                    szDisplay += "% " + m_ColModel.LstLastQuery[m_iRemoveSelectedIndex].GetIdealIdentifier();
                    szDisplay += " -> " + m_TBInAddCombo.Text;
                    // Replace
                    szCmdString += "% " + m_ColModel.LstLastQuery[m_iRemoveSelectedIndex].GetFullIdentifier();
                    szCmdString += " -> ";
                    szCmdString += m_TBInAddCombo.Text;
                    LstTextChanges.Add(szDisplay);
                    DataModel.LstTextChanges.Add(szCmdString);
                }
                else
                {
                    // Remove
                    // Use the selected index to get the correct card from the collection model
                    szCmdString += "- " + m_ColModel.LstLastQuery[m_iRemoveSelectedIndex].GetFullIdentifier();
                    szDisplay += "- "  + m_ColModel.LstLastQuery[m_iRemoveSelectedIndex].GetIdealIdentifier();
                    LstTextChanges.Add(szDisplay);
                    DataModel.LstTextChanges.Add(szCmdString);
                }
                m_TBInAddCombo.Text = "";
                m_TBInRemoveCombo.Text = "";
                m_TBInRemoveCombo.Focus();
            }
        }

        public void eAddCardSearch_SelectionChanged(object sender, RoutedEventArgs e)
        {
            m_iAddSelectedIndex = CBAddCardSearch.SelectedIndex;
            if (CBAddCardSearch.SelectedIndex != -1)
            {
                m_bIsOfferedSelectionAdd = true;

                AddCardDisplay = CBAddCardSearch.SelectedValue.ToString();

                CBAddCardSearch.SelectionChanged -= eAddCardSearch_SelectionChanged;
                CBAddCardSearch.RemoveHandler(System.Windows.Controls.Primitives.TextBoxBase.TextChangedEvent,
                      new System.Windows.Controls.TextChangedEventHandler(eAddCardSearch_TextChanged));

                CBAddCardSearch.Items.Clear();
                CBAddCardSearch.Items.Add(AddCardDisplay);
                CBAddCardSearch.SelectedIndex = 0;

                CBAddCardSearch.AddHandler(System.Windows.Controls.Primitives.TextBoxBase.TextChangedEvent,
                      new System.Windows.Controls.TextChangedEventHandler(eAddCardSearch_TextChanged));
                CBAddCardSearch.SelectionChanged += eAddCardSearch_SelectionChanged;
            }
        }

        public void eCBAddCardSearch_LostFocus(object sender, RoutedEventArgs e)
        {
            CBAddCardSearch.IsDropDownOpen = false;
        }

        public void eCBAddCardSearch_Loaded(object sender, RoutedEventArgs e)
        {
            m_TBInAddCombo = (TextBox)CBAddCardSearch.Template.FindName("PART_EditableTextBox", CBAddCardSearch);
            m_TBInAddCombo.Focus();
        }

        public void eAddCardSearch_TextChanged(object sender, TextChangedEventArgs e)
        {
            CBAddCardSearch.RemoveHandler(System.Windows.Controls.Primitives.TextBoxBase.TextChangedEvent,
                      new System.Windows.Controls.TextChangedEventHandler(eAddCardSearch_TextChanged));
            string szDropDownText = CBAddCardSearch.Text;
            CBAddCardSearch.Items.Clear();
            m_TBInAddCombo.SelectionLength = 0;
            m_bIsOfferedSelectionAdd = false;
            if (szDropDownText.Length > 3)
            {
                CBAddCardSearch.IsDropDownOpen = true;

                List<string> lstCards = ServerInterfaceModel.GetAllCardsStartingWith(szDropDownText.ToLower());
                if (lstCards.Count > 0)
                {
                    foreach (string szCard in lstCards)
                    {
                        CBAddCardSearch.Items.Add(szCard);
                    }
                    
                }
                
            }
            m_TBInAddCombo.SelectionStart = CBAddCardSearch.Text.Length; // THIS IS NECESSARY
            m_TBInAddCombo.SelectionLength = 0;
            CBAddCardSearch.AddHandler(System.Windows.Controls.Primitives.TextBoxBase.TextChangedEvent,
                      new System.Windows.Controls.TextChangedEventHandler(eAddCardSearch_TextChanged));
        }

        public void eRemoveCardSearch_SelectionChanged(object sender, RoutedEventArgs e)
        {
            m_iRemoveSelectedIndex = CBRemoveCardSearch.SelectedIndex;
            if (CBRemoveCardSearch.SelectedIndex != -1)
            {
                m_bIsOfferedSelectionRemove = true;

                RemoveCardDisplay = m_ColModel.LstLastQuery[m_iRemoveSelectedIndex].GetIdealIdentifier();

                CBRemoveCardSearch.SelectionChanged -= eRemoveCardSearch_SelectionChanged;
                CBRemoveCardSearch.RemoveHandler(System.Windows.Controls.Primitives.TextBoxBase.TextChangedEvent,
                      new System.Windows.Controls.TextChangedEventHandler(eRemoveCardSearch_TextChanged));

                CBRemoveCardSearch.Items.Clear();
                CBRemoveCardSearch.Items.Add(RemoveCardDisplay);
                CBRemoveCardSearch.SelectedIndex = 0;

                CBRemoveCardSearch.AddHandler(System.Windows.Controls.Primitives.TextBoxBase.TextChangedEvent,
                      new System.Windows.Controls.TextChangedEventHandler(eRemoveCardSearch_TextChanged));
                CBRemoveCardSearch.SelectionChanged += eRemoveCardSearch_SelectionChanged;
            }
        }

        public void eCBRemoveCardSearch_LostFocus(object sender, RoutedEventArgs e)
        {
            CBRemoveCardSearch.IsDropDownOpen = false;
        }

        public void eCBRemoveCardSearch_Loaded(object sender, RoutedEventArgs e)
        {
            m_TBInRemoveCombo = (TextBox)CBRemoveCardSearch.Template.FindName("PART_EditableTextBox", CBRemoveCardSearch);
        }

        public void eRemoveCardSearch_TextChanged(object sender, TextChangedEventArgs e)
        {
            CBRemoveCardSearch.RemoveHandler(System.Windows.Controls.Primitives.TextBoxBase.TextChangedEvent,
                      new System.Windows.Controls.TextChangedEventHandler(eRemoveCardSearch_TextChanged));
            
            string szDropDownText = CBRemoveCardSearch.Text;
            CBRemoveCardSearch.Items.Clear();
            m_TBInRemoveCombo.SelectionLength = 0;
            m_bIsOfferedSelectionRemove = false;
            if (szDropDownText.Length > 3)
            {
                CBRemoveCardSearch.IsDropDownOpen = true;
                List<string> lstCards = m_ColModel.GetAllCardsStartingWith(szDropDownText.ToLower());
                foreach (string szCard in lstCards)
                {
                    CBRemoveCardSearch.Items.Add(szCard);
                }
            }
            m_TBInRemoveCombo.SelectionStart = CBRemoveCardSearch.Text.Length; // THIS IS NECESSARY
            m_TBInRemoveCombo.SelectionLength = 0;
            CBRemoveCardSearch.AddHandler(System.Windows.Controls.Primitives.TextBoxBase.TextChangedEvent,
                      new System.Windows.Controls.TextChangedEventHandler(eRemoveCardSearch_TextChanged));
        }

        private void eCancel_Click(object sender, RoutedEventArgs e)
        {
            UnhandledEvent(DataModel, "Cancel");
        }

        private void eAccept_Click(object sender, RoutedEventArgs e)
        {
            UnhandledEvent(DataModel, "Accept");
        }

        private void eTabOnAddCB_Press(object o, KeyEventArgs e)
        {
            if (e.Key == Key.Tab && CBAddCardSearch.Items.Count > 0)
            {
                CBAddCardSearch.SelectedIndex = 0;
            }
        }

        private void eTabOnRemoveCB_Press(object o, KeyEventArgs e)
        {
            if (e.Key == Key.Tab && CBRemoveCardSearch.Items.Count > 0)
            {
                CBRemoveCardSearch.SelectedIndex = 0;
            }
        }

        #endregion


    }
}