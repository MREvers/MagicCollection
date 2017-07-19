using StoreFrontPro.Views.Components.PlusMinusControl;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Media;

namespace StoreFrontPro.Views.Interfaces.CollectionChanger
{
    class VMCollectionEditorItem : ViewModel<MCollectionEditorItem>
    {
        public string DisplayText
        {
            get { return Model.DisplayText; }
        }

        public ObservableCollection<string> PossibleValues { get; set; } = new ObservableCollection<string>();

        public string Set
        {
            get { return Model.SelectedSet; }
            set { Model.SelectedSet = value; }
        }

        public Brush DisplayColor { get; set; }

        public VPlusMinusControl PlusMinusControl { get; set; }

        public VMCollectionEditorItem(MCollectionEditorItem Model) : base(Model)
        {
            if (Model.FunctionText[0] == '+')
            {
                DisplayColor = new LinearGradientBrush(Colors.LightBlue, Colors.White, 45);
            }
            else if (Model.FunctionText[0] == '%')
            {
                GradientStopCollection gsc = new GradientStopCollection();
                gsc.Add(new GradientStop(Colors.MistyRose, 0));
                gsc.Add(new GradientStop(Colors.White, .5));
                gsc.Add(new GradientStop(Colors.LightBlue, 1));
                DisplayColor = new LinearGradientBrush(gsc, 45);
            }
            else if (Model.FunctionText[0] == '-')
            {
                DisplayColor = new LinearGradientBrush( Colors.MistyRose, Colors.White, 45);
            }

            PossibleValues = new ObservableCollection<string>(Model.SetOptions);
            Set = PossibleValues?[0] ?? "";
            VMPlusMinusControl plusMinusControlVM = new VMPlusMinusControl(Model.BoxedAmount);
            PlusMinusControl = new VPlusMinusControl() { DataContext = plusMinusControlVM };
        }
    }
}
