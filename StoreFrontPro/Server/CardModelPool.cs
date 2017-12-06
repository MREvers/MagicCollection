using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StoreFrontPro.Server
{
   class CardPoolObject
   {
      public CardModel Model;
      public bool Free;
      public int Index;

      public CardPoolObject(int aiIndex)
      {
         Index = aiIndex;
      }
   }

   class CardModelPool
   {
      static uint POOL_SIZE = 100;
      static CardModelPool instance = null;
      static CardPoolObject[] ModelPool = new CardPoolObject[POOL_SIZE];

      public CardModelPool() 
      {
         for( int i = 0; i < POOL_SIZE; i++ )
         {
            ModelPool[i] = new CardPoolObject(i);
         }
      }

      public CardModel GetModel(string aszIdentifier, string aszParent)
      {
         CardModel cardRetval = null;
         for( int i = 0; i < POOL_SIZE; i++ )
         {
            if( ModelPool[i].Free )
            {
               cardRetval = new CardModel(aszIdentifier, aszParent);
            }
         }

         if( cardRetval == null )
         {
            // Exception
         }

         return cardRetval;
      }

      private int getAlreadyPooled()
      {
         return 1;
      }

      public static CardModelPool Handler
      {
         get
         {
            if( instance == null )
            {
               instance = new CardModelPool();
            }
            return instance;
         }
      }
   }
}
