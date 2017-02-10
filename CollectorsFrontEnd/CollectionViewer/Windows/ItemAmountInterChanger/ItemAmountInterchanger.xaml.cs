using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace CollectorsFrontEnd
{
    /// <summary>
    /// Interaction logic for ItemAmountInterchanger.xaml
    /// </summary>
    public partial class ItemAmountInterchanger : UserControl
    {
        public string ActiveCard { get; set; }
        public string ActiveCollection { get; set; }
        public int ICountOf = 0;

        public ItemAmountInterchanger()
        {
            InitializeComponent();
        }

        public void SetCard(string aszCollection, string aszCard, int iCount)
        {
            LblCardName.Content = aszCard;

            ActiveCard = aszCard;
            ActiveCollection = aszCollection;
            ICountOf = iCount;
            PopulateColCounts();
        }

        public void PopulateColCounts()
        {
            Dictionary<string, int> GeneralizationsList = new Dictionary<string, int>();

            // Add code here to fetch generalization amounts. Until then...


            List<List<Tuple<string, string>>> lstTags = MainWindow.SCI.GetMetaTags(ActiveCollection, ActiveCard);
            int iCopyCount = 0;
            // Go through and look for generalizations on this copy.
            foreach (List<Tuple<string, string>> TagList in lstTags)
            {
                foreach (Tuple<string, string> Tuple in TagList)
                {
                    if (Tuple.Item1 == "Generalization")
                    {
                        if (GeneralizationsList.ContainsKey(Tuple.Item2))
                        {
                            GeneralizationsList[Tuple.Item2] += 1;
                        }
                        else
                        {
                            GeneralizationsList[Tuple.Item2] = 1;
                        }
                        if (Tuple.Item2.ToLower() != "main")
                        {
                            iCopyCount++;
                        }

                    }
                }
            } // End Sort generalizations
            CObjectListDisplay COCardParse = new CObjectListDisplay();
            COCardParse.SetCard(ActiveCard);

            GeneralizationsList["Main"] = COCardParse.iCount - iCopyCount;

            foreach (string szGeneralization in GeneralizationsList.Keys)
            {
                AmountInterchanger AI = new AmountInterchanger();
                AI.SetName(szGeneralization);
                AI.TBAMount.Text = ICountOf.ToString();
                SPCollections.Children.Add(AI);
            }
        }
    }
}
