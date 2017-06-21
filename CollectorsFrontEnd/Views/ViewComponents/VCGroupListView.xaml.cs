using CollectorsFrontEnd.InterfaceModels;
using CollectorsFrontEnd.StoreFrontSupport;
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

namespace CollectorsFrontEnd.Views.ViewComponents
{
    /// <summary>
    /// View Component (VC) Data Context is usually set by the constructing object.
    /// Interaction logic for VCGroupList.xaml
    /// </summary>
    public partial class VCGroupListView : UserControl, IView
    {
        #region Fields
        private CollectionModel m_oViewCollection;
        private string m_szGroupingAttribute;

        // We can come up with a better solution in the future
        private List<string> m_lstPriorityGroupingKeys = new List<string>()
        {
            "G",
            "R",
            "W",
            "U",
            "B"
        };
        #endregion

        #region Events
        public event DisplayEventHandler DisplayEvent;
        #endregion

        #region Public Methods
        public VCGroupListView(CollectionModel CollectionToView)
        {
            InitializeComponent();

            m_oViewCollection = CollectionToView;
            m_szGroupingAttribute = "colorIdentity";
        }

        public List<object> GetViewHook(ViewComponentHandles ViewHandle)
        {
            switch (ViewHandle)
            {
                case ViewComponentHandles.Buttons:
                    return new List<object>() { DisplayEvent };
                default:
                    return null;
            }
        }

        public void Update()
        {
            if (DataContext != null)
            {
                refreshGroupDisplay();
            }
        }
        #endregion

        #region Private Methods

        // This functions sorts each of the cards by group. The VCIGrouplist class is responsible
        // for sorting and displaying the cards correctly.
        public void refreshGroupDisplay()
        {
            // Eventually use reflection to get this property. This property is bound in XAML
            // so it is already required by this class. This will de-tether the type requirement.
            VMCube oContext = DataContext as VMCube;
            oContext.Groups.Clear();
            Dictionary<string, List<CardModel>> lstLists = generateGroupLists();
            List<VCIGroupList> lstPriorityGroups = new List<VCIGroupList>();
            List<VCIGroupList> lstNonPriorityGroups = new List<VCIGroupList>();
            foreach (string szKey in lstLists.Keys)
            {
                VCIGroupList newGroup = new VCIGroupList(szKey, lstLists[szKey]);
                if (m_lstPriorityGroupingKeys.Where(x => newGroup.VMVCIGroupList.GroupName.Contains(x)).Count() == 1)
                {
                    lstPriorityGroups.Add(newGroup);
                }
                else
                {
                    lstNonPriorityGroups.Add(newGroup);
                }
            }

            foreach (var ng in lstPriorityGroups)
            {
                oContext.Groups.Add(ng);
            }
            foreach (var ng in lstNonPriorityGroups)
            {
                oContext.Groups.Add(ng);
            }

            //eResizeGroups();
        }


        private void eResizeGroups()
        {
            // Eventually use reflection to get this property. This property is bound in XAML
            // so it is already required by this class. This will de-tether the type requirement.
            VMCube oContext = DataContext as VMCube;
            oContext.Groups.Clear();
            foreach (VCIGroupList oGroup in oContext.Groups)
            {
                if (m_lstPriorityGroupingKeys.Where(x => oGroup.VMVCIGroupList.GroupName.Contains(x)).Count() == 1)
                {
                    oGroup.Height = this.ActualHeight;
                }
            }
        }

        private Dictionary<string, List<CardModel>> generateGroupLists()
        {
            Dictionary<string, List<CardModel>> MapGroups = new Dictionary<string, List<CardModel>>();
            foreach (CardModel oCardModel in m_oViewCollection.CollectionItems)
            {
                string szAttribValue = oCardModel.GetAttr(m_szGroupingAttribute);
                if (MapGroups.ContainsKey(szAttribValue))
                {
                    MapGroups[szAttribValue].Add(oCardModel);
                }
                else
                {
                    MapGroups[szAttribValue] = new List<CardModel>() { oCardModel };
                }
            }

            return MapGroups;
        }

        #endregion

        #region UI Event Handlers
        private void eMainDisplay_Resize(object sender, SizeChangedEventArgs e)
        {
            DisplayEventArgs eventDetails = new DisplayEventArgs(
                Source: "VCGroupList", 
                Property: "MainDisplay",
                Event: "Resized");
            DisplayEvent(Source: this, Event: eventDetails);
        }
        #endregion
    }
}
