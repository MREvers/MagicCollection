using StoreFrontPro.Server;
using StoreFrontPro.Tools;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;

namespace StoreFrontPro.Views.CollectionViews.Cube
{
    class VMCardGroupDisplay : ViewModel<ObservableCollection<CardModel>>
    {

        public ObservableCollection<VCardGroupList> CategoryGroups { get; set; } = new ObservableCollection<VCardGroupList>() { };

        // We can come up with a better solution in the future
        private List<string> m_lstPriorityGroupingKeys = new List<string>()
        {
            "W",
            "U",
            "B",
            "R",
            "G"
        };

        public VMCardGroupDisplay(ObservableCollection<CardModel> Model) : base(Model)
        {
            SyncWithModel();
            CollectionModel.RegisterCollectionListener(Model, (o, e) => { SyncWithModel(); });
        }

        public void SyncWithModel()
        {
            Action actWrapper = () => { SyncWithModel(); };
            if (!Application.Current.Dispatcher.CheckAccess())
            {
                Application.Current.Dispatcher.BeginInvoke(actWrapper);
                return;
            }
            inSyncWithModel();
        }

        private void inSyncWithModel()
        {
            // Get all of the grouping keys
            List<string> lstGroupingKeys = Model.Select(x => getCategory(x.GetAttr("manaCost"))).Distinct().ToList();

            List<VCardGroupList> lstRemoveGroups = new List<VCardGroupList>();
            foreach(var group in CategoryGroups)
            {
                ((VMCardGroupList)group.DataContext).SyncWithModel();
                if (((VMCardGroupList)group.DataContext).CategoryList.Count == 0)
                {
                    lstRemoveGroups.Add(group);
                }
            }

            foreach(var rmGroup in lstRemoveGroups)
            {
                CategoryGroups.Remove(rmGroup);
            }

            foreach (string category in lstGroupingKeys)
            {
                VCardGroupList oExistingDisplay = CategoryGroups.Where(x => ((VMCardGroupList)(x.DataContext)).GroupName == category).FirstOrDefault();
                if (oExistingDisplay == null)
                {
                    MCardGroupList oCardGroupList = new MCardGroupList("manaCost", category, m_lstPriorityGroupingKeys, Model);
                    if (oCardGroupList.GroupedList.Count > 0)
                    {
                        VMCardGroupList oCardGroupListVM = new VMCardGroupList(oCardGroupList, m_lstPriorityGroupingKeys.Contains(category));
                        VCardGroupList oCardGroupListV = new VCardGroupList() { DataContext = oCardGroupListVM };
                        CategoryGroups.Add(oCardGroupListV);
                        sortCategoryGroups();
                    }
                }
            }
        }

        private string getCategory(string szMan)
        {
            string szRetVal = "";
            foreach(string priorityCategory in m_lstPriorityGroupingKeys)
            {
                szRetVal += szMan.Contains(priorityCategory) ? (szRetVal == "" ? "" : "\\") + priorityCategory  : "";
            }
            return szRetVal;
        }

        private void sortCategoryGroups()
        {
            List<VCardGroupList> lstTemp = CategoryGroups.ToList();

            CategoryGroups.Clear();
            foreach(string category in m_lstPriorityGroupingKeys)
            {
                VCardGroupList lst = lstTemp.Where(x => ((VMCardGroupList)x.DataContext).GroupName == category).FirstOrDefault();
                if (lst != null)
                {
                    CategoryGroups.Add(lst);
                    lstTemp.Remove(lst);
                }
            }

            foreach(VCardGroupList VList in lstTemp)
            {
                string szGroup = ((VMCardGroupList)VList.DataContext).GroupName;

                int i = szGroup.Length > 0 ? 0 : CategoryGroups.Count();
                for (; i < CategoryGroups.Count(); i++)
                {
                    string inListGroup = ((VMCardGroupList)CategoryGroups[i].DataContext).GroupName;
                    if (szGroup.Length <= inListGroup.Length )
                    {
                        break;
                    }
                }

                if ( i == CategoryGroups.Count() - 1)
                {
                    CategoryGroups.Add(VList);
                }
                else
                {
                    CategoryGroups.Insert(i, VList);
                }
            }
        }
    }
}
