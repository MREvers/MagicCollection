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
    /// Interaction logic for CompSubEnterText.xaml
    /// </summary>
    public partial class CompSubEnterText : UserControl, IComponent
    {

        public class CompSubEnterTextDataModel: IDataModel
        {
            public string Text = "";
        }

        public string EnteredText { get; set; }

        public CompSubEnterTextDataModel DataModel = new CompSubEnterTextDataModel();

        public CompSubEnterText()
        {
            InitializeComponent();

            DataContext = this;
        }

        public event ComponentEvent UnhandledEvent;

        public IDataModel GetDataModel()
        {
            return DataModel;
        }

        public void RouteReceivedUnhandledEvent(IDataModel aDataObject, string aszAction)
        {
            throw new NotImplementedException();
        }

        private void eBtnOK_Click(object sender, RoutedEventArgs e)
        {
            DataModel.Text = EnteredText;
            UnhandledEvent(DataModel, "OK");
        }

        private void eBtnCancel_Click(object sender, RoutedEventArgs e)
        {
            UnhandledEvent(DataModel, "Cancel");
        }
    }
}
