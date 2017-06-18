using CollectorsFrontEnd.StoreFrontSupport;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;

namespace CollectorsFrontEnd.Views
{
    /// <summary>
    /// The Purpose of the view model is the prepare the data for the UI. Nothing More.
    /// </summary>
    public class VMCollectionsMenu : ViewModel
    {
        public string SelectedCollection { get; set; } = "";
        public ObservableCollection<string> CollectionPreview { get; set; } = new ObservableCollection<string>();
        public ObservableCollection<string> AvailableCollections { get; set; } = new ObservableCollection<string>();

        public VMCollectionsMenu(UserControl Model) : base(Model) { }
    }
}
