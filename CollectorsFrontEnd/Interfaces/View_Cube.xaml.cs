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
    public partial class View_Cube : UserControl, IMenuBarComponent
    {
        #region Data Binding

        public ObservableCollection<Module_CardGroupList> LstGroups { get; set; }

        #endregion

        #region Public events

        public event ComponentEvent UnhandledEvent;

        #endregion 

        #region PUblic Fields

        public CollectionModel DataModel;

        public string GroupingAttr;

        #endregion

        #region Private Fields
        private UserControl m_OverlayControl;
        private Module_CardGroupList m_LastClickedGroup = null;
        #endregion

        #region Public Functions

        public View_Cube(string aszCollectionName)
        {
            InitializeComponent();
            DataContext = this;
            if ((DataModel = ServerInterfaceModel.GetCollectionModel(aszCollectionName)) == null)
            {
                DataModel = ServerInterfaceModel.GenerateCollectionModel(aszCollectionName);
            }
            GroupingAttr = "colors";
            LstGroups = new ObservableCollection<Module_CardGroupList>();
            buildGroupsView();
        }

        public List<Tuple<string, MenuAction>> GetMenuActions()
        {
            List<Tuple<string, MenuAction>> LstMenuActions = new List<Tuple<string, MenuAction>>()
            {
                new Tuple<string, MenuAction>("Edit List", eShowBulkEdits),
                new Tuple<string, MenuAction>("Set Baseline History", eSetBaselineHistory)
            };
            return LstMenuActions;
        }

        public void RouteReceivedUnhandledEvent(IDataModel aoDataObject, string aszAction)
        {
            if (aoDataObject.GetType() == typeof(Module_BulkEdits.Data))
            {
                Module_BulkEdits.Data dM = (Module_BulkEdits.Data)aoDataObject;
                if (aszAction == "Cancel")
                {
                    showMainDisplay();
                }
                else if (aszAction == "Accept")
                {
                    ecBulkEditsAccept(dM);
                }
            }
            else if (aoDataObject.GetType() == typeof(Module_CardGroupList.Data))
            {
                Module_CardGroupList.Data DM = (Module_CardGroupList.Data)aoDataObject;
                if (aszAction == "Selection Changed")
                {
                    ecGroupsSelectionChanged(DM);
                }
            }
        }

        public IDataModel GetDataModel()
        {
            throw new NotImplementedException();
        }

        #endregion

        #region Private Functions

        private void buildGroupsView()
        {
            LstGroups.Clear();
            Dictionary<string, List<CardModel>> lstLists = generateGroupLists();
            foreach (string szKey in lstLists.Keys)
            {
                Module_CardGroupList newGroup = new Module_CardGroupList(szKey, lstLists[szKey]);
                newGroup.UnhandledEvent += RouteReceivedUnhandledEvent;
                LstGroups.Add(newGroup);
            }
        }

        private Dictionary<string, List<CardModel>> generateGroupLists()
        {
            Dictionary<string, List<CardModel>> MapGroups = new Dictionary<string, List<CardModel>>();
            foreach (CardModel cm in DataModel.LstCopyModels)
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

        private void showBulkEditWindow()
        {
            showMainDisplay();
            if (DataModel.CollectionName != "")
            {
                Module_BulkEdits ITI = new Module_BulkEdits(DataModel);
                m_OverlayControl = ITI;
                ITI.UnhandledEvent += RouteReceivedUnhandledEvent;
                Panel.SetZIndex(CenterPanel, 2);
                CenterPanel.Children.Add(ITI);
                MainDisplay.IsEnabled = false;
            }
        }

        private void showMainDisplay()
        {
            CenterPanel.Children.Remove(m_OverlayControl);
            m_OverlayControl = null;
            MainDisplay.IsEnabled = true;
        }

        #endregion

        #region Event Handlers

        private void ecBulkEditsAccept(Module_BulkEdits.Data aDataModel)
        {
            DataModel.SubmitBulkEdits(aDataModel.LstTextChanges);
            DataModel.Refresh();
            buildGroupsView();
            showMainDisplay();
        }

        private void ecGroupsSelectionChanged(Module_CardGroupList.Data aoDataModel)
        {

            foreach (var lstGroup in LstGroups)
            {
                if (lstGroup.GroupName != aoDataModel.SourceGroup.GroupName)
                {
                    lstGroup.ClearSelection();
                }
            }

        }

        #endregion

        #region UI Event Handlers

        private void Canvas_MouseMove(object sender, System.Windows.Input.MouseEventArgs e)
        {
           // var pos = e.GetPosition((Canvas)sender);
            //Canvas.SetLeft(eye, pos.X);
            //Canvas.SetTop(eye, pos.Y);
        }

        public void eSetBaselineHistory()
        {
            DataModel.SetBaselineHistory();
        }

        public void eShowBulkEdits()
        {
            showBulkEditWindow();
        }

        #endregion
    }
}
