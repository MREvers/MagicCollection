using CollectorsFrontEnd.InterfaceModels;
using CollectorsFrontEnd.Interfaces.Subs;
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

namespace CollectorsFrontEnd.Interfaces
{
    /// <summary>
    /// Interaction logic for CompCubeView.xaml
    /// </summary>
    public partial class CompCubeView : UserControl, IMenuBarComponent
    {
        #region Data Binding

        public ObservableCollection<CompSubCardGroupList> LstGroups { get; set; }

        #endregion

        #region Public events

        public event ComponentEvent UnhandledEvent;

        #endregion 

        #region PUblic Fields

        public CollectionModel DataModel;

        public string GroupingAttr;

        #endregion

        #region Public Functions

        public CompCubeView(string aszCollectionName)
        {
            InitializeComponent();
            DataContext = this;
            if ((DataModel = ServerInterfaceModel.GetCollectionModel(aszCollectionName)) == null)
            {
                DataModel = ServerInterfaceModel.GenerateCollectionModel(aszCollectionName);
            }
            GroupingAttr = "colors";
            LstGroups = new ObservableCollection<CompSubCardGroupList>();
            buildGroupsView();
        }

        public List<Tuple<string, MenuAction>> GetMenuActions()
        {
            //throw new NotImplementedException();
            return new List<Tuple<string, MenuAction>>();
        }

        public void RouteReceivedUnhandledEvent(IDataModel aDataObject, string aszAction)
        {
            throw new NotImplementedException();
        }

        public IDataModel GetDataModel()
        {
            throw new NotImplementedException();
        }

        #endregion

        #region Private Functions

        private void buildGroupsView()
        {
            Dictionary<string, List<CardModel>> lstLists = generateGroupLists();
            foreach(string szKey in lstLists.Keys)
            {
                LstGroups.Add(new CompSubCardGroupList(szKey, lstLists[szKey]));
            }
        }

        private Dictionary<string,List<CardModel>> generateGroupLists()
        {
            Dictionary<string, List<CardModel>> MapGroups = new Dictionary<string, List<CardModel>>();
            foreach(CardModel cm in DataModel.LstCopyModels)
            {
                string szKey = cm.GetAttr(GroupingAttr);
                if (MapGroups.ContainsKey(szKey))
                {
                    MapGroups[szKey].Add(cm);
                }
                else
                {
                    MapGroups[szKey] = new List<CardModel>() { cm };
                }
            }

            return MapGroups;
        }

        #endregion
    }
}
