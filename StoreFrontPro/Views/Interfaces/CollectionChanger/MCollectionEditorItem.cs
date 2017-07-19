using StoreFrontPro.Views.Components.PlusMinusControl;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StoreFrontPro.Views.Interfaces.CollectionChanger
{
    class MCollectionEditorItem
    {
        public int Amount
        {
            get { return BoxedAmount?.Value ?? 0; }
        }

        public List<string> SetOptions = new List<string>();
        public string SelectedSet;

        public MPlusMinusControl BoxedAmount;
        public string DisplayText;

        public string FunctionText;

        public MCollectionEditorItem(string DisplayText, string FunctionText, List<string> PossibleSetValues, int MaxOps = 99)
        {
            SetOptions = PossibleSetValues ?? new List<string>() { "" };
            SelectedSet = SetOptions[0];
            BoxedAmount = new MPlusMinusControl(1, MaxOps);
            this.DisplayText = DisplayText;
            this.FunctionText = FunctionText;
        }
    }
}
