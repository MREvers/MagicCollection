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
    /// Interaction logic for CompSubAmountChanger.xaml
    /// </summary>
    public partial class Module_AmountChanger : UserControl, IComponent, INotifyPropertyChanged
    {
        #region Bindings
        public string Title
        {
            get
            {
                return DataModel.Title;
            }
        }

        public string Count
        {
            get
            {
                return DataModel.CurrentAmount.ToString();
            }
            set
            {
                int iNewVal;
                if (int.TryParse(value, out iNewVal))
                {
                    DataModel.CurrentAmount = iNewVal;
                    OnPropertyChanged("Count");
                }

            }
        }

        public event PropertyChangedEventHandler PropertyChanged;
        protected virtual void OnPropertyChanged(string propertyName = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
        #endregion

        #region Nested TYpes
        public class Data : IDataModel
        {
            public string Title;
            public int CurrentAmount;
            public int StartAmount;
            public List<Tuple<string, string>> LstMetaTags;
        }
        #endregion

        #region Public Events
        public event ComponentEvent UnhandledEvent;
        #endregion

        #region Public Fields
        public Data DataModel;
        #endregion

        #region Public Functions
        public Module_AmountChanger(string aszTitle, int aiAmount, List<Tuple<string, string>> alstMeta)
        {
            InitializeComponent();
            DataContext = this;
            DataModel = new Data();
            DataModel.Title = aszTitle;
            DataModel.LstMetaTags = alstMeta;
            DataModel.StartAmount = aiAmount;
            DataModel.CurrentAmount = aiAmount;
        }

        public void RouteReceivedUnhandledEvent(IDataModel aDataObject, string aszAction)
        {
            throw new NotImplementedException();
        }

        public IDataModel GetDataModel()
        {
            return DataModel;
        }
        #endregion

        #region UI Event Handlers
        private void eBtnDecrement_Click(object sender, RoutedEventArgs e)
        {
            DataModel.CurrentAmount = Math.Max(0, DataModel.CurrentAmount - 1);
            OnPropertyChanged("Count");
        }

        private void eBtnIncrement_Click(object sender, RoutedEventArgs e)
        {
            DataModel.CurrentAmount++;
            OnPropertyChanged("Count");
        }
        #endregion
    }
}
