using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Media.Imaging;

namespace CollectorsFrontEnd.InterfaceModels
{
    public static class ServerInterfaceModel
    {
        public const string SZ_IMAGE_CACHE_PATH = @"./Images/";

        private static ServerClientInterface SCI = new ServerClientInterface();
        private static List<CollectionModel> LstCollectionModels = new List<CollectionModel>();

        #region Friend Interfaces
        public static class CollectionInterfaceModel
        {
            public static void SaveCollection(string aszCollectionName)
            {
                SCI.SaveCollection(aszCollectionName);
            }

            public static void AddItem(string aszCollectionName, string aszCardNameLong, List<Tuple<string,string>> lstMeta)
            {
                SCI.AddItem(aszCollectionName, aszCardNameLong, lstMeta);
            }

            public static void RemoveItem(string aszCollectionName, string aszCardNameLong, List<Tuple<string, string>> lstMeta)
            {
                SCI.RemoveItem(aszCollectionName, aszCardNameLong, lstMeta);
            }

            /// <summary>
            /// Calls the server for the most up to date list of copies.
            /// </summary>
            /// <param name="aszCollectionName"></param>
            public static void Refresh(string aszCollectionName)
            {
                if (GetCollectionModel(aszCollectionName) != null)
                {
                    // List of [ { CardNameLong, [Tags, ...] }, ... ]
                    List<Tuple<string, List<Tuple<string, string>>>> lstCards =
                        SCI.GetCollectionListWithMeta(aszCollectionName);
                    CollectionModel CMCurrent = LstCollectionModels.FirstOrDefault(x => x.CollectionName == aszCollectionName);
                    if (CMCurrent != null)
                    {
                        CMCurrent.BuildCopyModelList(lstCards);
                    }
                }
            }
        }

        // Deals with all interface calls at the card class and copy level
        public static class CardClassInterfaceModel
        {
            private class ImageDownloadedEventArgs: EventArgs
            {
                public ImageDownloadedEventArgs(CardModel aDataModel, EventArgs ae)
                {
                    DataModel = aDataModel;
                    e = ae;
                }
                public CardModel DataModel;
                public EventArgs e;
            }

            // Compares long name
            public static bool AreCardsSame(CardModel aoCardOne, CardModel aoCardTwo)
            {
                return SCI.IsSameIdentity(aoCardOne.CardNameLong, aoCardTwo.CardNameLong);
            }

            public static bool AreMetaTagsSame(List<Tuple<string,string>> alstTupOne, List<Tuple<string, string>> alstTupTwo)
            {
                return SCI.IsSameMetaTags(alstTupOne, alstTupTwo);
            }

            public static void AddMetaTag(string aszCollectionName,
                string aszLongCardName,
                string aszKey,
                string aszVal,
                List<Tuple<string,string>> aLstMatchMeta)
            {
                SCI.AddMetaTag(aszCollectionName, aszLongCardName, aszKey, aszVal, aLstMatchMeta);
            }

            public static void RemoveMetaTag(string aszCollectionName,
                string aszLongCardName,
                string aszKey,
                List<Tuple<string, string>> aLstMatchMeta)
            {
                SCI.RemoveMetaTag(aszCollectionName, aszLongCardName, aszKey, aLstMatchMeta);
            }

            // Passes the image to the callback.
            public static void DownloadAndCacheImage(EventHandler aeHandlerCallback, CardModel aoCardModel)
            {
                // Save in set

                string szMUID = aoCardModel.GetMetaTag("multiverseid");
                BitmapImage bi3;
                string szFilePath = SZ_IMAGE_CACHE_PATH + aoCardModel.CardName + ".jpg";
                if (!File.Exists(szFilePath))
                {
                    bi3 = new BitmapImage();
                    bi3.DownloadCompleted += aeHandlerCallback;
                    bi3.DownloadFailed += eModelImage_DownloadFailed;
                    bi3.BeginInit();
                    bi3.DownloadCompleted +=
                            ((object sender, EventArgs e) =>
                            { eModelImage_DownloadCompleted(sender, new ImageDownloadedEventArgs(aoCardModel, e)); });
                    if (!string.IsNullOrEmpty(szMUID))
                    {
                        bi3.UriSource =
                            new Uri(@"http://gatherer.wizards.com/Handlers/Image.ashx?multiverseid=" +
                            szMUID + @"&type=card", UriKind.RelativeOrAbsolute);
                    }
                    else
                    {
                        bi3.UriSource =
                            new Uri(@"http://gatherer.wizards.com/Handlers/Image.ashx?name=" +
                            aoCardModel.CardName + "&type=card", UriKind.RelativeOrAbsolute);
                    }
                    bi3.CacheOption = BitmapCacheOption.OnLoad;
                    bi3.EndInit();
                }
                else
                {
                    string szFullPath = Path.GetFullPath(szFilePath);
                    bi3 = new BitmapImage(new Uri(szFullPath, UriKind.RelativeOrAbsolute));
                    aeHandlerCallback(bi3, null);
                }

            }

            private static void eModelImage_DownloadCompleted(object sender, ImageDownloadedEventArgs ie)
            {
                EventArgs e = ie.e;
                CardModel cardModel = ie.DataModel;
                JpegBitmapEncoder encoder = new JpegBitmapEncoder();
                String photolocation = SZ_IMAGE_CACHE_PATH + cardModel.CardName + ".jpg";  //file name 
                encoder.Frames.Add(BitmapFrame.Create((BitmapImage)sender));

                if (!Directory.Exists(Path.GetDirectoryName(photolocation)))
                {
                    Directory.CreateDirectory(Path.GetDirectoryName(photolocation));
                }

                using (var filestream = new FileStream(photolocation, FileMode.Create))
                    encoder.Save(filestream);
            }

            private static void eModelImage_DownloadFailed(object sender, EventArgs e)
            {

            }
        }
        #endregion

        /// <summary>
        /// Creates a new collection model from data from the server, if that collection model
        ///  doesn't already exist. If it exists, it updates the model.
        /// </summary>
        /// <param name="aszCollectionFileName"></param>
        /// <returns></returns>
        public static CollectionModel GenerateCollectionModel(string aszCollectionFileName)
        {
            string szColName = SCI.LoadCollection(aszCollectionFileName);
            // List of [ { CardNameLong, [Tags, ...] }, ... ]
            List<Tuple<string, List<Tuple<string, string>>>> lstCards =
                SCI.GetCollectionListWithMeta(szColName);
            CollectionModel CMNew = new CollectionModel(szColName, lstCards);
            CollectionModel CMCurrent = LstCollectionModels.FirstOrDefault(x => x.CollectionName == szColName);
            if (CMCurrent == null)
            {
                LstCollectionModels.Add(CMNew);
            }
            else
            {
                CMCurrent.LstCopyModels = CMNew.LstCopyModels;
            }
            
            return CMNew;
        }

        public static CardModel GenerateCopyModel(string aszCardNameLong, string aszCollectionName, List<Tuple<string, string>> aLstMetaTags)
        {
            // Really, this SCI function just parses the long name.
            MCopyObject oParsed = ServerInterfaceModel.SCI.ConvertItemToCopyObject(aszCardNameLong);
            List<Tuple<string, string>> lstIdentifiedAttrs =
                ServerInterfaceModel.SCI.GetCardAttributes(aszCardNameLong);
            // We also need the rest identified attrs
            CardModel CopyM = new CardModel(
                oParsed.Name,
                aszCollectionName,
                oParsed.Attributes.Select(x => new Tuple<string, string>(x.Key,x.Value)).ToList(),
                lstIdentifiedAttrs,
                aLstMetaTags);
            CopyM.SetAuxData(oParsed.Amount, aszCardNameLong);

            return CopyM;
        }

        /// <summary>
        /// Returns the collection model if it exists. Null otherwise.
        /// </summary>
        /// <param name="aszCollectionName"></param>
        /// <returns></returns>
        public static CollectionModel GetCollectionModel(string aszCollectionName)
        {
            return LstCollectionModels.FirstOrDefault(x => x.CollectionName == aszCollectionName);
        }

        public static List<string> GetLoadedCollectionList()
        {
            return LstCollectionModels.Select(x => x.CollectionName).ToList();
        }

        public static List<string> GetAllCardsStartingWith(string aszStart)
        {
            return SCI.GetAllCardsStartingWith(aszStart);
        }

        public static void ImportJSONCollection()
        {
            SCI.ImportCollection();
        }
    }
}
