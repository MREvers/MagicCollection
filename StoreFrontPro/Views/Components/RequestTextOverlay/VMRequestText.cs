using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StoreFrontPro.Views.Components.RequestTextOverlay
{
    class VMRequestText : ViewModel<string>, IViewComponent
    {
        private string _DisplayText;

        public event DisplayEventHandler DisplayEvent;

        public string DisplayText
        {
            get { return _DisplayText; }
            set { _DisplayText = value; OnPropertyChanged(); }
        }

        public RelayCommand AcceptCommand { get; set; }
        public RelayCommand CancelCommand { get; set; }

        public VMRequestText(string Model) : base(Model)
        {
            DisplayText = Model;
            AcceptCommand = new RelayCommand(eAcceptCommand);
            CancelCommand = new RelayCommand(eCancelCommand);
        }

        private void eAcceptCommand(object canExecute)
        {
            DisplayEventArgs eventArgs = new DisplayEventArgs("VMRequestText", "AcceptButton", "Clicked");
            eventArgs.Add("Text", this.DisplayText);
            DisplayEvent(this, eventArgs);
        }

        private void eCancelCommand(object canExecute)
        {
            DisplayEventArgs eventArgs = new DisplayEventArgs("VMRequestText", "CancelButton", "Clicked");
            DisplayEvent(this, eventArgs);
        }

        public List<StoreFrontMenuItem> GetMenuItems()
        {
            throw new NotImplementedException();
        }
    }
}
