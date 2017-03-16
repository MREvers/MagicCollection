using System;
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
    public partial class CompSubBulkEdits : UserControl, IComponent, INotifyPropertyChanged
    {
        public ObservableCollection<string> LstTextChanges { get; set; }
        public string RemoveCardDisplay { get; set; }
        public string AddCardDisplay { get; set; }

        public class CompSubBulkEditsDataModel : IDataModel
        {
            public List<string> LstTextChanges = new List<string>();
        }

        public CompSubBulkEditsDataModel DataModel;
        private int m_iAddSelectedIndex = -1;
        private int m_iRemoveSelectedInxed = -1;
        private bool m_bIsOfferedSelectionAdd = false;
        private bool m_bIsOfferedSelectionRemove = false;
        private TextBox m_TBInAddCombo;
        private TextBox m_TBInRemoveCombo;
        private CollectionModel m_ColModel;

        public CompSubBulkEdits(CollectionModel aoColModel)
        {
            InitializeComponent();
            LstTextChanges = new ObservableCollection<string>();
            m_ColModel = aoColModel;
            DataContext = this;
            DataModel = new CompSubBulkEditsDataModel();

            CBAddCardSearch.AddHandler(System.Windows.Controls.Primitives.TextBoxBase.TextChangedEvent,
                      new System.Windows.Controls.TextChangedEventHandler(eAddCardSearch_TextChanged));
            CBAddCardSearch.SelectionChanged += eAddCardSearch_SelectionChanged;
            CBAddCardSearch.Focus();
            CBAddCardSearch.Loaded += eCBAddCardSearch_Loaded;
            CBAddCardSearch.LostFocus += eCBAddCardSearch_LostFocus;
            CBAddCardSearch.IsTextSearchEnabled = false;

            CBRemoveCardSearch.AddHandler(System.Windows.Controls.Primitives.TextBoxBase.TextChangedEvent,
                      new System.Windows.Controls.TextChangedEventHandler(eRemoveCardSearch_TextChanged));

            CBRemoveCardSearch.SelectionChanged += eRemoveCardSearch_SelectionChanged;
            CBRemoveCardSearch.Loaded += eCBRemoveCardSearch_Loaded;
            CBRemoveCardSearch.LostFocus += eCBRemoveCardSearch_LostFocus;
            CBRemoveCardSearch.IsTextSearchEnabled = false;
        }

        public event PropertyChangedEventHandler PropertyChanged;
        public event ComponentEvent UnhandledEvent;

        public IDataModel GetDataModel()
        {
            throw new NotImplementedException();
        }

        public void RouteReceivedUnhandledEvent(IDataModel aDataObject, string aszAction)
        {
            throw new NotImplementedException();
        }

        private void eAddItem_Click(object sender, RoutedEventArgs e)
        {
            if (m_bIsOfferedSelectionAdd)
            {
                string szCmdString = "";
                szCmdString += "+ " + m_TBInAddCombo.Text;
                LstTextChanges.Add(szCmdString);
                DataModel.LstTextChanges.Add(szCmdString);
            }
        }

        private void eRemoveReplaceItem_Click(object sender, RoutedEventArgs e)
        {
            if (m_bIsOfferedSelectionRemove)
            {
                string szCmdString = "";
                if (m_bIsOfferedSelectionAdd)
                {
                    // Replace
                    szCmdString += "% " + m_TBInAddCombo.Text;
                    szCmdString += " -> ";
                    szCmdString += m_ColModel.LstCopyModels[m_iRemoveSelectedInxed].GetFullIdentifier();
                    LstTextChanges.Add(szCmdString);
                    DataModel.LstTextChanges.Add(szCmdString);
                }
                else
                {
                    // Remove
                    // Use the selected index to get the correct card from the collection model
                    szCmdString += "- " + m_ColModel.LstCopyModels[m_iRemoveSelectedInxed].GetFullIdentifier();
                    LstTextChanges.Add(szCmdString);
                    DataModel.LstTextChanges.Add(szCmdString);
                }
            }
        }

        public void eAddCardSearch_SelectionChanged(object sender, RoutedEventArgs e)
        {
            m_iAddSelectedIndex = CBAddCardSearch.SelectedIndex;
            if (CBAddCardSearch.SelectedIndex != -1)
            {
                m_bIsOfferedSelectionAdd = true;

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
            CBAddCardSearch.IsDropDownOpen = true;
            string szDropDownText = CBAddCardSearch.Text;
            CBAddCardSearch.Items.Clear();
            m_TBInAddCombo.SelectionLength = 0;
            m_bIsOfferedSelectionAdd = false;

            if (szDropDownText.Length > 3)
            {
                List<string> lstCards = ServerInterfaceModel.GetAllCardsStartingWith(szDropDownText.ToLower());
                foreach (string szCard in lstCards)
                {
                    CBAddCardSearch.Items.Add(szCard);
                }
            }
            m_TBInAddCombo.SelectionStart = CBAddCardSearch.Text.Length; // THIS IS NECESSARY
            m_TBInAddCombo.SelectionLength = 0;
        }

        public void eRemoveCardSearch_SelectionChanged(object sender, RoutedEventArgs e)
        {
            m_iRemoveSelectedInxed = CBRemoveCardSearch.SelectedIndex;
            if (CBRemoveCardSearch.SelectedIndex != -1)
            {
                m_bIsOfferedSelectionRemove = true;

                RemoveCardDisplay = m_ColModel.LstCopyModels[m_iRemoveSelectedInxed].CardName;

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
            CBRemoveCardSearch.IsDropDownOpen = true;
            string szDropDownText = CBRemoveCardSearch.Text;
            CBRemoveCardSearch.Items.Clear();
            m_TBInRemoveCombo.SelectionLength = 0;
            m_bIsOfferedSelectionRemove = false;
            if (szDropDownText.Length > 3)
            {
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
    }
}
