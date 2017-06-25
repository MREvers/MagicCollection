using StoreFrontPro.Server;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StoreFrontPro.Views.CollectionViews.Cube
{
    class VMCardGroupDisplay : ViewModel<List<CardModel>>
    {

        public ObservableCollection<VCardGroupList> CategoryGroups { get; set; } = new ObservableCollection<VCardGroupList>() { };

        // We can come up with a better solution in the future
        private List<string> m_lstPriorityGroupingKeys = new List<string>()
        {
            "G",
            "R",
            "W",
            "U",
            "B"
        };

        public VMCardGroupDisplay(List<CardModel> Model) : base(Model)
        {
            SyncWithModel();
        }

        public void SyncWithModel()
        {
            foreach(string category in m_lstPriorityGroupingKeys)
            {
                VCardGroupList oExistingDisplay = CategoryGroups.Where(x => ((VMCardGroupList)(x.DataContext)).GroupName == category).FirstOrDefault();
                if (oExistingDisplay != null)
                {
                    ((VMCardGroupList)oExistingDisplay.DataContext).SyncWithModel();
                }
                else
                {
                    MCardGroupList oCardGroupList = new MCardGroupList("manaCost", category, Model);
                    if (oCardGroupList.GroupedList.Count > 0)
                    {
                        VMCardGroupList oCardGroupListVM = new VMCardGroupList(oCardGroupList);
                        VCardGroupList oCardGroupListV = new VCardGroupList();
                        oCardGroupListV.DataContext = new VMCardGroupList(oCardGroupList);
                        CategoryGroups.Add(oCardGroupListV);
                    }
                }
            }
        }
    }
}
