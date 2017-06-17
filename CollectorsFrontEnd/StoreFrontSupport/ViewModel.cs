using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;

namespace CollectorsFrontEnd.StoreFrontSupport
{
    public class ViewModel: INotifyPropertyChanged
    {
        protected UserControl _NestedControls;
        public UserControl NestedControls
        {
            get { return _NestedControls; }
            set { _NestedControls = value; OnPropertyChanged(); }
        }

        public event PropertyChangedEventHandler PropertyChanged;
        protected virtual void OnPropertyChanged([CallerMemberName] string propertyName = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }

        public ViewModel(UserControl Model)
        {
            NestedControls = Model;
        }
    }
}
