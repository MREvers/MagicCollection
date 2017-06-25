using StoreFrontPro.Server;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StoreFrontPro.Views.CollectionViews.Cube
{ 
    class VMCardGroupList : ViewModel<MCardGroupList>
    {
        public string GroupName { get; set; }
        public ObservableCollection<string> CategoryList { get; set; } = new ObservableCollection<string>() { };

        public VMCardGroupList(MCardGroupList Model) : base(Model)
        {
            GroupName = Model.ExpectedAttribute;
            SyncWithModel();
        }

        public void SyncWithModel()
        {
            CategoryList.Clear();
            Model.GroupedList.ForEach(x => CategoryList.Add(x.GetIdealIdentifier()));
        }
    }
}
