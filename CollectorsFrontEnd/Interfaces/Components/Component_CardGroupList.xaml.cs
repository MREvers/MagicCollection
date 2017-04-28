using CollectorsFrontEnd.InterfaceModels;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
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
    /// Interaction logic for CompSubCardGroupList.xaml
    /// </summary>
    public partial class Component_CardGroupList : UserControl, IComponent, INotifyPropertyChanged
    {
        #region Data Binding

        public ObservableCollection<ListViewItem> LstItems { get; set; }

        private string _GroupName;
        public string GroupName
        {
            get
            {
                return _GroupName;
            }
            set
            {
                _GroupName = value;
                OnPropertyChanged("GroupName");
            }
        }

        private Module_CardDisplayer _ToolTipDisplay;
        public Module_CardDisplayer ToolTipDisplay
        {
            get
            {
                return _ToolTipDisplay;
            }
            set
            {
                _ToolTipDisplay = value;
                OnPropertyChanged("ToolTipDisplay");
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;
        protected virtual void OnPropertyChanged(string propertyName)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
        #endregion

        #region Nested Types
        public class Data : IDataModel
        {
            public List<CardModel> LstItems = new List<CardModel>();
            public Component_CardGroupList SourceGroup;
        }
        #endregion

        #region Public Events
        public event ComponentEvent UnhandledEvent;
        #endregion

        #region Public properties

        public List<CardModel> LstCardModels = new List<CardModel>();

        public List<string> LstSortingAttrs = new List<string>() { "manaCost", "name" };

        #endregion

        #region public fields
        public Data DataModel;
        public string ToolTipName;
        #endregion

        #region Public Functions
        public Component_CardGroupList(string aszGroupName, List<CardModel> alstModels)
        {
            InitializeComponent();
            DataContext = this;

            ToolTipName = null;
            DataModel = new Data();
            DataModel.SourceGroup = this;

            GroupName = aszGroupName + " (" + alstModels.Count + ")";
            LstCardModels = alstModels;
            LstItems = new ObservableCollection<ListViewItem>();
            List<CardModel> lstSorted = new List<CardModel>();

            alstModels.Sort(CompareCMC);
            int iCmc = -1;
            string szCmc = alstModels.First().GetAttr("cmc");
            if (int.TryParse(szCmc, out iCmc))
            {

            }
            for (int i = 0; i < alstModels.Count; i++)
            {
                int newCmc;
                szCmc = alstModels[i].GetAttr("cmc");
                if (int.TryParse(szCmc, out newCmc))
                {
                    if (newCmc != iCmc)
                    {
                        if (iCmc != -1)
                        {
                            ListViewItem blankItem = new ListViewItem();
                            blankItem.Content = "";
                            blankItem.MouseLeave += eItemList_MouseLeave;
                            blankItem.MouseEnter += eItemList_MouseMove;
                            LstItems.Add(blankItem);
                        }

                        iCmc = newCmc;
                    }
                }
                ListViewItem newItem = new ListViewItem();
                newItem.Content = alstModels[i].GetIdealIdentifier();
                newItem.DataContext = this;
                newItem.MouseLeave += eItemList_MouseLeave;
                newItem.MouseEnter += eItemList_MouseMove;
                LstItems.Add(newItem);
            }
        }

        public void ClearSelection()
        {
            CardGroupList.SelectionChanged -= eItemList_SelectionChanged;
            CardGroupList.UnselectAll();
            CardGroupList.SelectionChanged += eItemList_SelectionChanged;
        }

        public IDataModel GetDataModel()
        {
            return DataModel;
        }

        public void RouteReceivedUnhandledEvent(IDataModel aDataObject, string aszAction)
        {
            throw new NotImplementedException();
        }

        #endregion

        #region Private Functions
        private int CompareCMC(CardModel x, CardModel y)
        {
            int xCmc;
            int yCmc;
            string xszCmc = x.GetAttr("cmc");
            string yszCmc = y.GetAttr("cmc");

            bool xNum = int.TryParse(xszCmc, out xCmc);
            bool yNum = int.TryParse(yszCmc, out yCmc);

            if (xNum && yNum)
            {
                if (xCmc == yCmc)
                {
                    return 0;
                }
                else
                {
                    return (xCmc > yCmc) ? 1 : -1;
                }
            }
            else if (xNum && !yNum)
            {
                return 1;
            }
            else if (!xNum && yNum)
            {
                return -1;
            }
            else
            {
                return 0;
            }
        }
        #endregion

        #region UI event handlers
        private void eItemList_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            Data DM = (Data)GetDataModel();
            if (UnhandledEvent != null)
            {
                UnhandledEvent(DM, "Selection Changed");
            }

        }

        private void eItemList_MouseMove(object sender, MouseEventArgs e)
        {
            ListViewItem item = (ListViewItem)sender;
            string ItemText = item.Content.ToString();
            if (ItemText == "")
            {
                ToolTipDisplay = null;
                return;
            }

            if (ToolTipName != ItemText)
            {
                ToolTipName = ItemText;
                CardModel oNewDisplay = LstCardModels.Where(x => x.GetIdealIdentifier() == ItemText).FirstOrDefault();
                if (ToolTipDisplay != null)
                {
                    ToolTipDisplay.LoadNewSet(oNewDisplay);
                }
                else
                {
                    ToolTipDisplay = new Module_CardDisplayer(oNewDisplay);
                }
            }
        }

        private void eItemList_MouseLeave(object sender, MouseEventArgs e)
        {
            ToolTipName = "";
        }

        #endregion 
    }
}
