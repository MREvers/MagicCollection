using System;
using System.Collections.Generic;
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

namespace CollectorsFrontEnd.Interfaces.Subs
{
    /// <summary>
    /// Interaction logic for CompSubEnterText.xaml
    /// </summary>
    public partial class Module_EnterText : UserControl, IComponent, INotifyPropertyChanged
    {

        #region Data Binding
        private string _EnteredText;
        public string EnteredText
        {
            get
            {
                return _EnteredText;
            }
            set
            {
                _EnteredText = value;
                OnPropertyChanged("EnteredText");
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;
        protected virtual void OnPropertyChanged(string propertyName)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
        #endregion

        #region Nested Types
        public class CompSubEnterTextDataModel: IDataModel
        {
            public string Text = "";
        }
        #endregion

        #region Public Events
        public event ComponentEvent UnhandledEvent;
        #endregion

        #region Public Fields

        public CompSubEnterTextDataModel DataModel = new CompSubEnterTextDataModel();
        #endregion

        #region Public Functions
        public Module_EnterText()
        {
            InitializeComponent();

            DataContext = this;
        }

        

        public IDataModel GetDataModel()
        {
            return DataModel;
        }

        public void RouteReceivedUnhandledEvent(IDataModel aDataObject, string aszAction)
        {
            throw new NotImplementedException();
        }
        #endregion

        #region UI Event Handelrs
        private void eBtnOK_Click(object sender, RoutedEventArgs e)
        {
            DataModel.Text = EnteredText;
            UnhandledEvent(DataModel, "OK");
        }

        private void eBtnCancel_Click(object sender, RoutedEventArgs e)
        {
            UnhandledEvent(DataModel, "Cancel");
        }
        #endregion
    }
}
