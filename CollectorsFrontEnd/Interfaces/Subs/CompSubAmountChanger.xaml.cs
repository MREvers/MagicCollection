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
    public partial class CompSubAmountChanger : UserControl, IComponent, INotifyPropertyChanged
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
        public class CompSubAmountChangerModel : IDataModel
        {
            public string Title;
            public int CurrentAmount;
            public int StartAmount;
            public List<Tuple<string, string>> LstMetaTags;
        }

        CompSubAmountChangerModel DataModel;

        public event ComponentEvent UnhandledEvent;

        public CompSubAmountChanger(string aszTitle, int aiAmount, List<Tuple<string, string>> alstMeta)
        {
            InitializeComponent();
            DataContext = this;
            DataModel = new CompSubAmountChangerModel();
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
    }
}
