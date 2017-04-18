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
    /// Interaction logic for CompSubKeyValCreater.xaml
    /// </summary>
    public partial class Module_KeyValCreater : UserControl, IComponent
    {


        public class Data : INotifyPropertyChanged, IDataModel
        {
            private string _Key = "";
            private string _Value = "";
            public string Key
            {
                get
                {
                    return _Key;
                }
                set
                {
                    _Key = value;
                    OnPropertyChanged("Key");
                }
            }
            public string Value
            {
                get
                {
                    return _Value;
                }
                set
                {
                    _Value = value;
                    OnPropertyChanged("Value");
                }
            }

            public event PropertyChangedEventHandler PropertyChanged;
            protected virtual void OnPropertyChanged(string propertyName = null)
            {
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
            }
        }

        public event ComponentEvent UnhandledEvent;

        public Data DataModel { get; set; }

        public Module_KeyValCreater()
        {
            DataModel = new Data();
            InitializeComponent();
            DataContext = DataModel;
        }

        public IDataModel GetDataModel()
        {
            throw new NotImplementedException();
        }

        public void RouteReceivedUnhandledEvent(IDataModel aDataObject, string aszAction)
        {
            throw new NotImplementedException();
        }

        private void eOK_Click(object sender, RoutedEventArgs e)
        {
            UnhandledEvent(DataModel, "OK");
        }

        private void eCancel_Click(object sender, RoutedEventArgs e)
        {
            UnhandledEvent(DataModel, "Cancel");
        }
    }
}
