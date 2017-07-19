using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StoreFrontPro.Views.Components.PlusMinusControl
{
    class VMPlusMinusControl : ViewModel<MPlusMinusControl>
    {
        public string Count
        {
            get { return Model.Value.ToString(); }
            set
            {
                int iCount;
                if (int.TryParse(value, out iCount))
                {
                    Model.Value = iCount;
                }
                
            }
        }

        public RelayCommand PlusCommand { get; set; }
        public RelayCommand MinusCommand { get; set; }

        public VMPlusMinusControl(MPlusMinusControl Model) : base(Model)
        {
            PlusCommand = new RelayCommand(ePlusCommand);
            MinusCommand = new RelayCommand(eMinusCommand);
        }

        private void ePlusCommand(object aoCanExecute)
        {
            Model.Value++;
            OnPropertyChanged("Count");
        }

        private void eMinusCommand(object aoCanExecute)
        {
            Model.Value = Math.Max(1, Model.Value - 1);
            OnPropertyChanged("Count");
        }
    }
}
