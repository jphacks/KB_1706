require 'ruby_coincheck_client'

class Api::CustomersController < ApplicationController
  before_action :set_api_customer, only: [:show, :update, :destroy]
  before_action :authenticate_api_user!
  # GET /api/customers
  # GET /api/customers.json
  def index
    @api_customers = Api::Customer.all
    render 'index', :formats => [:json], :handlers => [:jbuilder]
  end

  # GET /api/customers/1
  # GET /api/customers/1.json
  def show

    render :json => @api_customer
    #render 'show', :formats => [:json], :handlers => [:jbuilder]
  end

  # POST /api/customers
  # POST /api/customers.json
  def create
    json_request = JSON.parse(request.body.read)

    if !json_request.blank?
      personal = json_request

      @api_customer = Api::Customer.create!(json_request)
      
      render :json => @api_customer.id
    

    # if @api_customer.save
    #   render json: @api_customer, status: :created
    # else
    #   render json: @api_customer.errors, status: :unprocessable_entity
    # end

    else
      personal = {'status' => 500}
    end
  end

  def pay
    json_request = JSON.parse(request.body.read)
    if !json_request.blank?
      @api_customer = Api::Customer.find(json_request["id"])
      p "#{@api_customer.onepass}"
      #ここにcoincheckapiを書く
      cc = CoincheckClient.new(@api_customer.onepass, @api_customer.twopass)
      #佐古さんのcoincheck address
      response = cc.create_send_money(address: "1GwnQa2b2yfLPBLnEQQHAjDLC5z2G2k4V7", amount: json_request["amount"])

      render :json => response
    
    else
      render :json => {'status' => 500}
    end
  end

  # PATCH/PUT /api/customers/1
  # PATCH/PUT /api/customers/1.json
  def update
    if @api_customer.update(api_customer_params)
      render :show, status: :ok, location: @api_customer
    else
      render json: @api_customer.errors, status: :unprocessable_entity
    end
  end

  # DELETE /api/customers/1
  # DELETE /api/customers/1.json
  def destroy
    @api_customer.destroy
  end

  private
    # Use callbacks to share common setup or constraints between actions.
    def set_api_customer
      @api_customer = Api::Customer.find(params[:id])
    end

    # Never trust parameters from the scary internet, only allow the white list through.
    def api_customer_params
      params.require(:api_customer).permit(:coinadress, :onepass, :twopass)
    end
end
