using CollectorsFrontEnd.InterfaceModels;
using CollectorsFrontEnd.StoreFrontSupport;
using CollectorsFrontEnd.Views.ViewComponents;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;

namespace CollectorsFrontEnd.Views
{
    public class VMCube : ViewModel
    {
        public CollectionModel ViewCollection { get; set; }
        public ObservableCollection<VCIGroupList> Groups { get; set; } = new ObservableCollection<VCIGroupList>();

        public VMCube(UserControl Model) : base(Model) { }

    }
}
