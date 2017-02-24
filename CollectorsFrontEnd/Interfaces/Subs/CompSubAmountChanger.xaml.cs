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
    /// Interaction logic for CompSubAmountChanger.xaml
    /// </summary>
    public partial class CompSubAmountChanger : UserControl, IComponent
    {
        public string Title { get
            {
                return DataModel.Title;
            }
        }

        public class CompSubAmountChangerModel: IDataModel
        {
            public string Title;
            public int StartAmount;
            public int EndAmount;
        }

        CompSubAmountChangerModel DataModel;

        public event ComponentEvent UnhandledEvent;

        public CompSubAmountChanger(string aszTitle, int aiAmount)
        {
            InitializeComponent();
            DataContext = this;
            DataModel = new CompSubAmountChangerModel();
            DataModel.Title = aszTitle;
            DataModel.EndAmount = aiAmount;
            DataModel.StartAmount = aiAmount;
        }

        public void RouteReceivedUnhandledEvent(IDataModel aDataObject, string aszAction)
        {
            throw new NotImplementedException();
        }

        public IDataModel GetDataModel()
        {
            return DataModel;
        }
    }
}
