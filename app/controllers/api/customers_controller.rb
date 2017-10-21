class Api::CustomersController < ApplicationController
  before_action :set_api_customer, only: [:show, :update, :destroy]

  # GET /api/customers
  # GET /api/customers.json
  def index
    @api_customers = Api::Customer.all
  end

  # GET /api/customers/1
  # GET /api/customers/1.json
  def show
  end

  # POST /api/customers
  # POST /api/customers.json
  def create
    @api_customer = Api::Customer.new(api_customer_params)

    if @api_customer.save
      render :show, status: :created, location: @api_customer
    else
      render json: @api_customer.errors, status: :unprocessable_entity
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
