using CollectorsFrontEnd.InterfaceModels;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
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
    /// Interaction logic for CompSubCardGroupList.xaml
    /// </summary>
    public partial class CompSubCardGroupList : UserControl, IComponent
    {
        public ObservableCollection<string> LstItems { get; set; }

        public string GroupName { get; set; }

        public List<CardModel> LstCardModels = new List<CardModel>();

        public List<string> LstSortingAttrs = new List<string>() { "manaCost","name" };

        public class CompSubCardGroupListDataModel: IDataModel
        {
            public List<CardModel> LstItems = new List<CardModel>( );
        }

        public CompSubCardGroupList(string aszGroupName, List<CardModel> alstModels)
        {
            InitializeComponent();
            DataContext = this;
            GroupName = aszGroupName + " (" + alstModels.Count + ")";
            LstCardModels = alstModels;
            LstItems = new ObservableCollection<string>();
            for (int i = 0; i< alstModels.Count; i++)
            {
                LstItems.Add(alstModels[i].GetIdealIdentifier());
            }
            
        }

        public event ComponentEvent UnhandledEvent;

        public IDataModel GetDataModel()
        {
            throw new NotImplementedException();
        }

        public void RouteReceivedUnhandledEvent(IDataModel aDataObject, string aszAction)
        {
            throw new NotImplementedException();
        }
    }
}
