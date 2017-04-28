using CollectorsFrontEnd.InterfaceModels;
using CollectorsFrontEnd.Interfaces.Controls;
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
using System.Diagnostics;

namespace CollectorsFrontEnd.Interfaces
{
    /// <summary>
    /// Interaction logic for CompCubeView.xaml
    /// </summary>
    public partial class View_Cube : UserControl, IMenuBarComponent
    {
        #region Data Binding

        public ObservableCollection<Component_CardGroupList> LstGroups { get; set; }

        #endregion

        #region Public events

        public event ComponentEvent UnhandledEvent;

        #endregion

        #region Public Properties

        public Component_GroupView MainDisplay
        {
            get
            {
                return (Component_GroupView) m_Overlay.MainControl;
            }
        }

        #endregion

        #region Public Fields

        public CollectionModel DataModel;

        public string GroupingAttr;

        // We can come up with a better solution in the future
        public List<string> LstSpecialGroups = new List<string>()
        {
            "G",
            "R",
            "W",
            "U",
            "B"
        };

        #endregion

        #region Private Fields
        private Control_OverlayPanel m_Overlay = null;
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
            GroupingAttr = "colorIdentity";
            LstGroups = new ObservableCollection<Component_CardGroupList>();
            Loaded += (o, e) =>
            {
                buildGroupsView();
            };

            Component_GroupView groupView = new Component_GroupView();
            groupView.LstGroups = LstGroups;
            groupView.ResizeOccured += eMainDisplay_Resize;

            Control_OverlayPanel mainPanel = new Control_OverlayPanel();
            mainPanel.SetMainControl(groupView);
            m_Overlay = mainPanel;

            CenterPanel.Children.Add(m_Overlay);

        }

        public List<Tuple<string, MenuAction>> GetMenuActions()
        {
            List<Tuple<string, MenuAction>> LstMenuActions = new List<Tuple<string, MenuAction>>()
            {
                new Tuple<string, MenuAction>("Save Collection", eSaveCollection),
                new Tuple<string, MenuAction>("Separator", null),
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
            else if (aoDataObject.GetType() == typeof(Component_CardGroupList.Data))
            {
                Component_CardGroupList.Data DM = (Component_CardGroupList.Data)aoDataObject;
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

        // This must be called after the window is loaded or "ActualHeight" will be 0.
        private void buildGroupsView()
        {
            Stopwatch sp = new Stopwatch();
            sp.Start();

            LstGroups.Clear();
            Dictionary<string, List<CardModel>> lstLists = generateGroupLists();
            List<Component_CardGroupList> lstFirstsList = new List<Component_CardGroupList>();
            List<Component_CardGroupList> lstPostLists = new List<Component_CardGroupList>();
            foreach (string szKey in lstLists.Keys)
            {
                Component_CardGroupList newGroup = new Component_CardGroupList(szKey, lstLists[szKey]);
                newGroup.UnhandledEvent += RouteReceivedUnhandledEvent;
                if (LstSpecialGroups.Where(x => newGroup.GroupName.Contains(x)).Count() == 1)
                {
                    lstFirstsList.Add(newGroup);
                }
                else
                {
                    lstPostLists.Add(newGroup);
                }
            }

            foreach (var ng in lstFirstsList)
            {
                LstGroups.Add(ng);
            }
            foreach (var ng in lstPostLists)
            {
                LstGroups.Add(ng);
            }
            ecResizeGroups();

            sp.Stop();
            long itime = sp.ElapsedMilliseconds;
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
                ITI.UnhandledEvent += RouteReceivedUnhandledEvent;
                m_Overlay.ShowOverlay(ITI);
            }
        }

        private void showMainDisplay()
        {
            m_Overlay.ShowMain();
        }

        #endregion

        #region Event Handlers

        private void ecResizeGroups()
        {
            foreach (Component_CardGroupList grp in LstGroups)
            {
                if (LstSpecialGroups.Where(x => grp.GroupName.Contains(x)).Count() == 1)
                {
                    grp.Height = MainDisplay.ActualHeight;
                }
            }
        }

        private void ecBulkEditsAccept(Module_BulkEdits.Data aDataModel)
        {
            DataModel.SubmitBulkEdits(aDataModel.LstTextChanges);
            DataModel.Refresh();
            buildGroupsView();
            showMainDisplay();
        }

        private void ecGroupsSelectionChanged(Component_CardGroupList.Data aoDataModel)
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

        public void eSetBaselineHistory()
        {
            DataModel.SetBaselineHistory();
        }

        private void eSaveCollection()
        {
            DataModel.SaveCollection();
        }

        public void eShowBulkEdits()
        {
            showBulkEditWindow();
        }

        private void eMainDisplay_Resize(object sender, SizeChangedEventArgs e)
        {
            ecResizeGroups();
        }

        #endregion
    }
}
