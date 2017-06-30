using StoreFrontPro.Server;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;

namespace StoreFrontPro.Views.CollectionViews.Cube
{
    class VMCardGroupDisplay : ViewModel<List<CardModel>>
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

        public VMCardGroupDisplay(List<CardModel> Model) : base(Model)
        {
            SyncWithModel();
        }

        public void SyncWithModel()
        {
            inSyncWithModel();
        }

        private void inSyncWithModel()
        {
            // Get all of the grouping keys
            List<string> lstGroupingKeys = Model.Select(x => getCategory(x.GetAttr("manaCost"))).Distinct().ToList();

            foreach (string category in lstGroupingKeys)
            {
                VCardGroupList oExistingDisplay = CategoryGroups.Where(x => ((VMCardGroupList)(x.DataContext)).GroupName == category).FirstOrDefault();
                if (oExistingDisplay != null)
                {
                    ((VMCardGroupList)oExistingDisplay.DataContext).SyncWithModel();
                    if (((VMCardGroupList)oExistingDisplay.DataContext).CategoryList.Count == 0)
                    {
                        CategoryGroups.Remove(oExistingDisplay);
                    }
                }
                else
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
                int i = 0;
                for (; i < CategoryGroups.Count(); i++)
                {
                    if (((VMCardGroupList)CategoryGroups[i].DataContext).GroupName.Length > ((VMCardGroupList)VList.DataContext).GroupName.Length)
                    {
                        break;
                    }
                }

                if ( i == CategoryGroups.Count() - 1)
                {
                    i++;
                }

                CategoryGroups.Insert(i, VList);
            }
        }
    }
}
