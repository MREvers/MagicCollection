#pragma once

template<typename T>
class TryGet
{
public:
   TryGet()
   {
      m_FoundValue = nullptr;
   }

   ~TryGet() {};

   void Set(T* aptValue)
   {
      m_FoundValue = aptValue;
   }

   bool Good() const
   {
      return m_FoundValue != nullptr;
   }

   T* Value()
   {
      return m_FoundValue;
   }

   T* operator ->()
   {
      return Value();
   }

private:
   T* m_FoundValue;
};