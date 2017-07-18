using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Media.Effects;
using System.Windows.Shapes;

namespace StoreFrontPro.Views.Components.ManaViewer
{
    class VMManaViewer: ViewModel<string>
    {
        public ObservableCollection<UIElement> Symbols { get; set; } = new ObservableCollection<UIElement>();

        public VMManaViewer(string Model) : base(Model)
        {
            processSymbols();
        }

        public void processSymbols()
        {
            List<string> lstSymbols = Model.Split('{').Select(x => x.Trim('}', ' ')).Where(x => !string.IsNullOrEmpty(x)).ToList();
            foreach(string symbol in lstSymbols)
            {
                processSymbol(symbol);
            }
        }

        public void processSymbol(string symbol)
        {
            int iNum;
            if (int.TryParse(symbol, out iNum))
            {
                // Its a number
                Grid symbolGrid = new Grid();
                Ellipse newEllipseSymbol = new Ellipse() { Width = 14, Height = 14, StrokeThickness = .5, Stroke = new SolidColorBrush(Colors.Black) };
                newEllipseSymbol.Fill = new SolidColorBrush(Colors.Gainsboro);
                symbolGrid.Children.Add(newEllipseSymbol);
                TextBlock newTextBlock = new TextBlock();
                newTextBlock.VerticalAlignment = VerticalAlignment.Center;
                newTextBlock.HorizontalAlignment = HorizontalAlignment.Center;
                newTextBlock.FontWeight = FontWeights.Bold;
                newTextBlock.Text = iNum.ToString();
                symbolGrid.Children.Add(newTextBlock);
                Symbols.Add(symbolGrid);
            }
            else
            {
                // Not a number.
                List<string> possibleSymbols = symbol.Split('/').ToList();
                Ellipse newEllipseSymbol = new Ellipse() { Width = 14, Height = 14, StrokeThickness = .5, Stroke = new SolidColorBrush(Colors.Black) };
                if (possibleSymbols.Count > 1)
                {
                    // Hybrid
                    Color color1 = getColor(possibleSymbols[0]);
                    Color color2 = getColor(possibleSymbols[1]);
                    GradientStopCollection gradient = new GradientStopCollection();
                    gradient.Add(new GradientStop(color1, 0));
                    gradient.Add(new GradientStop(color1, .69));
                    gradient.Add(new GradientStop(color2, .69));
                    gradient.Add(new GradientStop(color2, 1));
                    LinearGradientBrush splitBrush = new LinearGradientBrush(gradient, 45);
                    newEllipseSymbol.Fill = splitBrush;
                }
                else
                {
                    // Normal mana symbol
                    newEllipseSymbol.Fill = new SolidColorBrush(getColor(symbol));
                }
                newEllipseSymbol.Effect = new DropShadowEffect() { Color=Colors.Black, Direction=-45, RenderingBias= RenderingBias.Performance, BlurRadius=0, ShadowDepth=2};
                Symbols.Add(newEllipseSymbol);
            }
        }

        private Color getColor(string szColor)
        {
            Color cRetVal = Colors.White;
            if (szColor == "R")
            {
                cRetVal = Colors.Crimson;
            }
            else if (szColor == "W")
            {
                cRetVal = Colors.Beige;
            }
            else if (szColor == "G")
            {
                cRetVal = Colors.Green;
            }
            else if (szColor == "U")
            {
                cRetVal = Colors.DodgerBlue;
            }
            else if (szColor == "B")
            {
                cRetVal = Colors.DimGray;
            }
            return cRetVal;
        }
    }
}
