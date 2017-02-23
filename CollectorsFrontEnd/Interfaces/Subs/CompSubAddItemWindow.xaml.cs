using CollectorsFrontEnd.InterfaceModels;
using System;
using System.Collections.Generic;
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

namespace CollectorsFrontEnd.Interfaces.Subs
{
    /// <summary>
    /// Interaction logic for CompSubAddItemWindow.xaml
    /// </summary>
    public partial class CompSubAddItemWindow : UserControl, IComponent
    {
        

        public class AddItemDataModel: IDataModel
        {
            public string ComboBoxText { get; set; }
        }

        public event ComponentEvent UnhandledEvent;
        private TextBox m_TBInCombo;
        public AddItemDataModel DataModel;
        #region Public Functions

        public CompSubAddItemWindow()
        {
            InitializeComponent();
            DataContext = this;
            DataModel = new AddItemDataModel();
            CBCardSearch.AddHandler(System.Windows.Controls.Primitives.TextBoxBase.TextChangedEvent,
                      new System.Windows.Controls.TextChangedEventHandler(eComboBox_TextChanged));
            CBCardSearch.Focus();
            CBCardSearch.Loaded += eCBCardSearch_Loaded;
            CBCardSearch.IsTextSearchEnabled = false;
        }

        public void RouteReceivedUnhandledEvent(IDataModel aDataObject, string aszAction)
        {
            throw new NotImplementedException();
        }
        #endregion PUblic Functions
        #region UI Event Handlers
        private void eBtnAddCard_Click(object sender, RoutedEventArgs e)
        {
            UnhandledEvent(DataModel, "AddItem");
        }

        private void eBtnCancel_Click(object sender, RoutedEventArgs e)
        {
            UnhandledEvent(DataModel, "Cancel");
        }

        public void eCBCardSearch_Loaded(object sender, RoutedEventArgs e)
        {
            m_TBInCombo = (TextBox)CBCardSearch.Template.FindName("PART_EditableTextBox", CBCardSearch);
            m_TBInCombo.Focus();
        }

        public void eComboBox_TextChanged(object sender, TextChangedEventArgs e)
        {
            CBCardSearch.IsDropDownOpen = true;
            string szDropDownText = CBCardSearch.Text;
            DataModel.ComboBoxText = szDropDownText;
            CBCardSearch.Items.Clear();
            m_TBInCombo.SelectionLength = 0;

            if (szDropDownText.Length > 3)
            {
                List<string> lstCards = ServerInterfaceModel.GetAllCardsStartingWith(szDropDownText.ToLower());
                foreach (string szCard in lstCards)
                {
                    CBCardSearch.Items.Add(szCard);
                }
            }
            m_TBInCombo.SelectionStart = DataModel.ComboBoxText.Length; // THIS IS NECESSARY
            m_TBInCombo.SelectionLength = 0;
        }
        #endregion UI Event Handlers

    }
}
